// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/application.hpp>

#include<cstdlib>
#include<cstring>

#include<exception>
#include<algorithm>
#include<iostream>
#include<fstream>

#include<boost/thread.hpp>
#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>
#include<boost/lexical_cast.hpp>

#include<OpenEXR/ImfThreading.h>

#include<QApplication>

#include<ramen/version.hpp>

#include<ramen/app/preferences.hpp>
#include<ramen/app/document.hpp>

#include<ramen/memory/manager.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/image/init_image_processing.hpp>
#include<ramen/imageio/factory.hpp>

#include<ramen/render/render_thread.hpp>
#include<ramen/render/render_sequence.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/util/command_line_parser.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/dialogs/splash_screen.hpp>

namespace ramen
{

application_t *g_app = 0;

application_t::application_t( int argc, char **argv) : system_(), preferences_()
{
    RAMEN_ASSERT( g_app == 0 );
    g_app = this;

    max_threads_ = 0;
    img_cache_size_ = 0;
    command_line_ = false;
    render_mode_ = false;
    quitting_ = false;

    cmd_parser_.reset( new util::command_line_parser_t( argc, argv));

    google::InitGoogleLogging( cmd_parser_->argv[0]);
    //google::SetLogDestination( google::INFO, "filename.log");

    // Create QApplication
    QApplication *q_app = new QApplication( cmd_parser_->argc, cmd_parser_->argv);
    boost::filesystem::path bundle_path( system().app_bundle_path());
    bundle_path /= "lib/Qt_plugins";
    qApp->setLibraryPaths( QStringList( QString( ramen::filesystem::file_cstring( bundle_path))));

    parse_command_line( cmd_parser_->argc, cmd_parser_->argv);

    if( !system().simd_type() & system::simd_sse2)
        fatal_error( "No SSE2 instruction set, exiting", true);

    // init prefs
    preferences_.reset( new preferences_t());

    create_dirs();

    if( max_threads_ == 0)
        max_threads_ = boost::thread::hardware_concurrency();

    task_scheduler_.initialize( max_threads_);
    Imf::setGlobalThreadCount( max_threads_);

    if( !command_line_)
    {
        splash_.reset( new ui::splash_screen_t());
        splash_->show();
        splash_->show_message( RAMEN_NAME_VERSION_STR);
    }

    // init memory manager
    if( img_cache_size_ == 0)
    {
        boost::uint64_t percent  = preferences().max_image_memory();
        boost::uint64_t ram_size = system().ram_size();
        img_cache_size_ = ram_size / (boost::uint64_t) 100 * percent;
    }

    mem_manager_.reset( new memory::manager_t( img_cache_size_));

    if( !command_line_)
        splash_->show_message( "Initializing builtin nodes");
    node_factory_t::instance();

    if( !command_line_)
        splash_->show_message( "Initializing image processing");
    image::init_image_processing();

    if( !command_line_)
        splash_->show_message( "Initializing imageio");
    imageio::factory_t::instance();

    if( !command_line_)
        splash_->show_message( "Initializing OpenColorIO");
    ocio_manager_.reset( new ocio::manager_t());

    if( !command_line_)
        splash_->show_message( "Initializing render thread");
    render_thread_.init();

    if( !command_line_)
    {
        splash_->show_message( "Initializing user interface");
        ui_.reset( new ui::user_interface_t());
        ui_->init();
        print_app_info();
    }
}

application_t::~application_t()
{
    //	TODO: implement this.
    //delete_tmp_files();

    google::ShutdownGoogleLogging();
}

void application_t::create_dirs()
{
    // home
    boost::filesystem::path base( system().app_user_path());
    boost::filesystem::create_directories( base / "ocio");
    boost::filesystem::create_directories( base / "prefs");
    boost::filesystem::create_directories( base / "ui");

    // tmp
    boost::filesystem::create_directories( preferences().tmp_dir());
}

int application_t::run()
{
    if( !command_line_)
    {
        ui()->show();
        splash_->finish( ui()->main_window());
        splash_.reset();
        return ui()->run( infile_);
    }
    else
    {
        if( render_mode_)
        {
            try
            {
                open_document( infile_);
            }
            catch( std::exception& e)
            {
                fatal_error( e.what(), true);
            }

            if( !start_frame_)
                start_frame_ = document().composition().start_frame();

            if( !end_frame_)
                end_frame_ = document().composition().end_frame();

            if( !proxy_level_)
                proxy_level_ = 0;

            if( !subsample_)
                subsample_ = 1;

            if( !mb_extra_samples_)
                mb_extra_samples_ = 0;

            if( !mb_shutter_factor_)
                mb_shutter_factor_ = 1.0f;

            render::render_sequence( document().composition(), start_frame_.get(), end_frame_.get(),
                                     proxy_level_.get(), subsample_.get(), mb_extra_samples_.get(), mb_shutter_factor_.get());
        }
    }

    return 0;
}

// command line things
bool application_t::matches_option( char *arg, const char *opt) const
{
    if( !strcmp( arg, opt))
        return true;

    return false;
}

boost::optional<int> application_t::parse_int( int num, int argc, char **argv) const
{
    RAMEN_ASSERT( num > 0);
    RAMEN_ASSERT( argc > 0);

    boost::optional<int> result;

    if( num < argc)
    {
        try
        {
            result = boost::lexical_cast<int>( argv[num]);
        }
        catch( std::exception& e) {}
    }

    return result;
}

boost::optional<float> application_t::parse_float( int num, int argc, char **argv) const
{
    RAMEN_ASSERT( num > 0);
    RAMEN_ASSERT( argc > 0);

    boost::optional<float> result;

    if( num < argc)
    {
        try
        {
            result = boost::lexical_cast<float>( argv[num]);
        }
        catch( std::exception& e) {}
    }

    return result;
}

void application_t::parse_input_file( char *arg)
{
    // input file name
    infile_ = boost::filesystem::path( arg);

    if( infile_.is_relative())
        infile_ = boost::filesystem::absolute( infile_);
}

bool application_t::parse_common_option( int argc, char **argv, int& num)
{
    if( matches_option( argv[num], "-threads"))
    {
        boost::optional<int> op( parse_int( num + 1, argc, argv));

        if( op)
        {
            if( op.get() > 0)
                max_threads_ = op.get();
            else
                error( "threads should be equal or greater than 1. Ignoring", true);
        }
        else
        {
            fatal_error( "no number of threads given.", true);
            return false;
        }

        num += 2;
        return true;
    }
    else
    {
        if( argv[num][0] == '-')
            fatal_error( std::string( "unknown option: ") + argv[num], true);
    }

    return false;
}

void application_t::parse_command_line( int argc, char **argv)
{
    if( argc == 1)
        return;

    if( matches_option( argv[1], "-help") || matches_option( argv[1], "-h"))
        usage();

    if( matches_option( argv[1], "-version"))
    {
        std::cout << RAMEN_NAME_FULL_VERSION_STR << ", " << __DATE__ << std::endl;
        std::exit( 0);
    }

    int i = 1;
    while (i < argc)
    {
        if( matches_option( argv[i], "-render"))
        {
            command_line_ = true;
            render_mode_ = true;
            ++i;
            parse_render_command_line( argc, argv, i);
            return;
        }
        else if( parse_common_option( argc, argv, i))
            ;
        else
        {
            parse_input_file( argv[i]);
            ++i;
        }
    }
}

void application_t::parse_render_command_line( int argc, char **argv, int num)
{
    int i = num;
    while (i < argc)
    {
        if( matches_option( argv[i], "-help") || matches_option( argv[i], "-h"))
            render_usage();
        else if( matches_option( argv[i], "-frames"))
        {
            start_frame_ = parse_int( i + 1, argc, argv);
            end_frame_ = parse_int( i + 2, argc, argv);

            if( !start_frame_ || !end_frame_)
                render_usage();

            i += 3;
        }
        // TODO: add more options here...
        else if( parse_common_option( argc, argv, i))
        {
        }
        else
        {
            parse_input_file( argv[i]);
            ++i;
        }
    }

    if( infile_.empty())
        fatal_error( "No composition file given", true);

    // TODO: check everything needed here!
}

void application_t::usage()
{
    std::cout <<	RAMEN_NAME_FULL_VERSION_STR << ", " << __DATE__ << "\n" <<
                    "Usage: ramen [options] file...\n\n"
                    "Options:\n"
                    "-help, -h:       Print help message and exit.\n"
                    "-version:        Print version number and exit.\n"
                    "-threads n:      Use n threads.\n\n"
                    "-render:         Render composition. Run ramen -render -help for more information.\n"
                    << std::endl;
    std::exit( 0);
}

void application_t::render_usage()
{
    std::cout <<	RAMEN_NAME_FULL_VERSION_STR << ", " << __DATE__ << "\n" <<
                    "Usage: ramen -render [options] file...\n\n"
                    "Options:\n"
                    "-help, -h:       Print this help message and exit.\n"
                    "-threads n:      Use n threads.\n\n"
                    "-frames n m:     Render frames n to m.\n"
                    << std::endl;
    std::exit( 0);
}

void application_t::print_app_info()
{
    std::cout << RAMEN_NAME_FULL_VERSION_STR << ", " << __DATE__ << std::endl;
    std::cout << "System = " << system().system_name() << std::endl;

    switch( system().cpu_type())
    {
        case system::cpu_x86:
        {
            std::cout << "CPU = Intel compatible\n";

            #ifdef RAMEN_SSE_SUPPORT
                int sset = system().simd_type();

                if( sset == system::simd_none)
                    std::cout << "SIMD = None\n";
                else
                {
                    std::cout << "SIMD = SSE ";

                    if( sset & system::simd_sse2)
                        std::cout << "SSE2 ";

                    if( sset & system::simd_sse3)
                        std::cout << "SSE3 ";

                    std::cout << std::endl;
                }
            #endif
        }
        break;

        default:
            std::cout << "CPU = Unknown\n";
    }

    std::cout << "Using " << max_threads_ << " threads\n";
    std::cout << "Ram Size = " << system().ram_size() / 1024 / 1024 << " Mb\n";
    std::cout << "Image Cache Memory = " << mem_manager_->image_allocator().max_size() / 1024 / 1024 << " Mb\n";
}

// document handling
void application_t::create_new_document()
{
    delete_document();
    document_.reset( new document_t());
}

void application_t::open_document( const boost::filesystem::path& p)
{
    create_new_document();
    boost::filesystem::ifstream ifs( p, serialization::yaml_iarchive_t::file_open_mode());

    if( !ifs.is_open() || !ifs.good())
        throw std::runtime_error( std::string( "Couldn't open input file ") + filesystem::file_string( p));

    std::auto_ptr<serialization::yaml_iarchive_t> in;
    in.reset( new serialization::yaml_iarchive_t( ifs));

    if( !in->read_composition_header())
        throw std::runtime_error( std::string( "Couldn't read file header ") + filesystem::file_string( p));

    document().set_file( p);
    document().load( *in);

    std::string err = in->errors();

    if( !err.empty())
    {
        // TODO: display errors here
        // multiline_alert_t::Instance().show_alert( "Errors during file open", err);
    }
}

void application_t::delete_document()
{
    document_.reset( 0);
    memory_manager().clear_caches();
}

// messages
void application_t::fatal_error( const std::string& message, bool no_gui) const
{
    if( !command_line_ && ui() && !ui()->rendering() && !no_gui)
        ui()->fatal_error( message);
    else
    {
        std::cerr << "Fatal error: " << message << "\n";
        DLOG( FATAL) << message;
        abort();
    }
}

void application_t::error( const std::string& message, bool no_gui) const
{
    if( !command_line_ && ui() && !ui()->rendering() && !no_gui)
        ui()->error( message);
    else
    {
        std::cerr << "Error: " << message << "\n";
        DLOG( ERROR) << message;
    }
}

void application_t::inform( const std::string& message, bool no_gui) const
{
    if( !command_line_ && ui() && !ui()->rendering() && !no_gui)
        ui()->inform( message);
    else
    {
        std::cerr << "Info: " << message << "\n";
        DLOG( INFO) << message;
    }
}

bool application_t::question( const std::string& what, bool default_answer) const
{
    if( !command_line_ && ui() && !ui()->rendering())
        return ui()->question( what, default_answer);
    else
    {
        if( default_answer)
            std::cout << "Ramen, question: " << what << ", replying yes by default\n";
        else
            std::cout << "Ramen, question: " << what << ", replying no by default\n";
    }

    return default_answer;
}

application_t& app()
{
    RAMEN_ASSERT( g_app);
    return *g_app;
}

} // ramen
