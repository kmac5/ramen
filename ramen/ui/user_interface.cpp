// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/user_interface.hpp>

#include<utility>
#include<iostream>

#include<boost/foreach.hpp>
#include<boost/thread.hpp>
#include<boost/filesystem/operations.hpp>

#include<QApplication>
#include<QSplashScreen>
#include<QMessageBox>
#include<QFile>
#include<QFileDialog>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>
#include<QKeyEvent>
#include<QPlastiqueStyle>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>
#include<ramen/app/preferences.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/memory/manager.hpp>

#include<ramen/movieio/factory.hpp>

#include<ramen/render/image_node_renderer.hpp>

#include<ramen/ui/main_window.hpp>
#include<ramen/ui/palette.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/dialogs/multiline_alert.hpp>
#include<ramen/ui/dialogs/render_composition_dialog.hpp>
#include<ramen/ui/widgets/time_slider.hpp>

#include<ramen/serialization/yaml_iarchive.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{
namespace ui
{

user_interface_t::user_interface_t() : QObject()
{
    active_ = 0;
	context_ = 0;
	rendering_ = false;
	cancelled_ = false;
	interacting_ = false;
	event_filter_installed_ = false;

    image_types_str_ = "Image Files (";

	    BOOST_FOREACH( const std::string& ext, movieio::factory_t::instance().extensions())
	    {
			image_types_str_.append( "*.");
			image_types_str_.append( ext.c_str());
			image_types_str_.append( " ");
	    }

	image_types_str_.append( ")");

    viewer_ = 0;
    inspector_ = 0;
    anim_editor_ = 0;
    window_ = 0;
}

user_interface_t::~user_interface_t()
{
    viewer_->deleteLater();
    inspector_->deleteLater();
    anim_editor_->deleteLater();
}

void user_interface_t::init()
{
    init_ui_style();

    create_new_document();

    viewer_ = new viewer_t();
    inspector_ = new inspector_t();
    anim_editor_ = new anim_editor_t();
    window_ = new main_window_t();

    // restore the last saved window state
	restore_window_state();
}

void user_interface_t::init_ui_style()
{
	QPlastiqueStyle *style = new QPlastiqueStyle();
	qApp->setStyle( style);

	// init palette
	palette_t::instance();
}

void user_interface_t::save_window_state()
{
    /*
    QByteArray window_state = window_->saveState();
    boost::filesystem::path p = app().system().preferences_path() / "wstate.ui";
    QFile file( filesystem::file_cstring( p));

    if( file.open( QIODevice::WriteOnly))
        file.write( window_state);
    */
}

void user_interface_t::restore_window_state()
{
    /*
    boost::filesystem::path p = app().system().preferences_path() / "wstate.ui";
	{
	    QFile file(filesystem::file_cstring( p));

	    if( file.open( QIODevice::ReadOnly))
	    {
			QByteArray window_state = file.readAll();
			window_->restoreState( window_state);
			return;
	    }
	}
    */
}

void user_interface_t::show() { window_->show();}

int user_interface_t::run( const boost::filesystem::path& p)
{ 
	if( !p.empty())
		open_document( p);

	return qApp->exec();
}

void user_interface_t::quit()
{
    app().set_quitting( true);
    save_window_state();
    qApp->quit();
}

void user_interface_t::create_new_document()
{
    set_active_node( 0);
    set_context_node( 0);

    if( anim_editor_ )
    	anim_editor().clear_all();
	
	app().create_new_document();

    app().document().composition().attach_add_observer( boost::bind( &user_interface_t::node_added, this, _1));
    app().document().composition().attach_release_observer( boost::bind( &user_interface_t::node_released, this, _1));

    render_composition_dialog_t::instance().set_frame_range( app().document().composition().start_frame(),
																app().document().composition().end_frame());

    render_composition_dialog_t::instance().set_mblur_settings( 0, 1);    
    update();
}

void user_interface_t::open_document( const boost::filesystem::path& p)
{
    create_new_document();
    boost::filesystem::ifstream ifs( p, serialization::yaml_iarchive_t::file_open_mode());

    if( !ifs.is_open() || !ifs.good())
    {
        error( std::string( "Couldn't open input file ") + filesystem::file_string( p));
		update();
        return;
    }

	std::auto_ptr<serialization::yaml_iarchive_t> in;
	
	try
	{
		in.reset( new serialization::yaml_iarchive_t( ifs));
		
		if( !in->read_composition_header())
		{
			create_new_document();
			error( std::string( "Couldn't open input file ") + filesystem::file_string( p));
			update();
			return;
		}
		
		app().document().set_file( p);
		app().document().load( *in);
		main_window()->update_recent_files_menu( p);
	}
	catch( std::exception& e)
	{
		create_new_document();
        error( std::string( "Couldn't open input file ") + filesystem::file_string( p));
		update();
		return;
	}

	// read here ui info

	// update the dialogs
    render_composition_dialog_t::instance().set_frame_range( app().document().composition().start_frame(),
                                                    app().document().composition().end_frame());

    render_composition_dialog_t::instance().set_mblur_settings( 0, 1);

    update();
	std::string err = in->errors();
	
	if( !err.empty())
		multiline_alert_t::instance().show_alert( "Errors during file open", err);
}

bool user_interface_t::save_document()
{
    RAMEN_ASSERT( app().document().has_file());

	try
	{
		serialization::yaml_oarchive_t out;
		out.write_composition_header();
		app().document().save( out);
		write_ui_state( out);

		out.write_to_file( app().document().file());
		app().document().set_dirty( false);
	}
	catch( std::exception& e)
	{
		error( std::string( "Couldn't save file. Exception, what = ") + e.what());
		return false;
	}
	
	return true;
}

void user_interface_t::set_active_node( node_t *n)
{
    if( active_ != n)
    {
        if( active_)
            active_->end_active();

        active_ = n;

        if( active_)
            active_->begin_active();

        inspector().edit_node( n);
        viewer().set_active_node( n);
        anim_editor().set_active_node( n);
    }
}

void user_interface_t::set_context_node( node_t *n)
{
    if( context_ != n)
    {
        if( context_)
            context_->end_context();

        context_ = n;

        if( context_)
            context_->begin_context();

        viewer().set_context_node( n);
    }
}

void user_interface_t::node_added( node_t *n)
{
    viewer().node_added( n);
}

void user_interface_t::node_released( node_t *n)
{
    if( n == active_)
    {
        set_active_node( 0);
        update();
    }

    if( n == context_)
    {
        set_context_node( 0);
        update();
    }

    viewer().node_released( n);
}

void user_interface_t::update()
{
    if( !app().quitting())
	{
        if( window_)
            window_->update();

		update_anim_editors();
	}
}

void user_interface_t::begin_interaction()
{
	app().document().composition().begin_interaction();
	viewer().begin_interaction();
    interacting_ = true;
    app().memory_manager().begin_interaction();
}

void user_interface_t::end_interaction()
{ 
    interacting_ = false;
    app().memory_manager().end_interaction();
	viewer().end_interaction();
	app().document().composition().end_interaction();
}

int user_interface_t::start_frame() const
{
    return app().document().composition().start_frame();
}

int user_interface_t::end_frame() const
{
    return app().document().composition().end_frame();
}

float user_interface_t::frame() const
{
    return app().document().composition().frame();
}

void user_interface_t::set_start_frame( int t)
{
    app().document().composition().set_start_frame( t);
    main_window()->time_slider().update( app().document().composition().start_frame(),
										 app().document().composition().frame(),
										 app().document().composition().end_frame());

    render_composition_dialog_t::instance().set_frame_range( app().document().composition().start_frame(),
														    app().document().composition().end_frame());
}

void user_interface_t::set_end_frame( int t)
{
    app().document().composition().set_end_frame( t);
    main_window()->time_slider().update( app().document().composition().start_frame(),
										 app().document().composition().frame(),
										 app().document().composition().end_frame());

    render_composition_dialog_t::instance().set_frame_range( app().document().composition().start_frame(),
														    app().document().composition().end_frame());
}

void user_interface_t::set_frame( int t)
{
    app().document().composition().set_frame( t);
    main_window()->time_slider().update( app().document().composition().start_frame(),
										 app().document().composition().frame(),
										 app().document().composition().end_frame());

    inspector().update();
	update_anim_editors();
    viewer().frame_changed();
}

void user_interface_t::update_anim_editors()
{
    if( anim_editor_)
        anim_editor().update();
}

void user_interface_t::fatal_error( const std::string& msg) const
{
    QMessageBox::critical( 0, "Fatal Error", msg.c_str());
}

void user_interface_t::error( const std::string& msg) const
{
    QMessageBox::warning( ( QWidget *) main_window(), "Error", msg.c_str());
}

void user_interface_t::inform( const std::string& msg) const
{
    QMessageBox::information( ( QWidget *) main_window(), "Info", msg.c_str());
}

bool user_interface_t::question( const std::string& what, bool default_answer) const
{
    QMessageBox::StandardButton result;

    if( default_answer)
    {
        result = QMessageBox::question( ( QWidget *)  main_window(), "Question", what.c_str(),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return result == QMessageBox::Yes;
    }
    else
    {
        result = QMessageBox::question( ( QWidget *) main_window(), "Question", what.c_str(),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        return result == QMessageBox::Yes;
    }
}

bool user_interface_t::image_sequence_file_selector( boost::filesystem::path& p, bool& sequence, bool& relative) const
{
    std::string types( image_types_string().toStdString());
    return image_sequence_file_selector( "Open Image", types, p, sequence, relative);
}

bool user_interface_t::image_sequence_file_selector( const std::string& title, const std::string& types,
						       boost::filesystem::path& p, bool& sequence, bool& relative) const
{
    static bool was_relative = false;
    static bool was_sequence = true;

    QFileDialog dialog( 0, title.c_str(), QString::null, types.c_str());
	dialog.setOption( QFileDialog::DontUseNativeDialog, true);
    dialog.setFileMode( QFileDialog::ExistingFile);

    QWidget *extra_widgets = new QWidget( &dialog);
    QHBoxLayout *layout = new QHBoxLayout( extra_widgets);

    QCheckBox *relative_check = new QCheckBox( "Relative");

    if( app().document().has_file())
    {
        RAMEN_ASSERT( !app().document().composition().composition_dir().empty());
        relative_check->setChecked( was_relative);
    }
    else
    {
        was_relative = false;
        relative_check->setChecked( false);
        relative_check->setEnabled( false);
    }

    QCheckBox *sequence_check = new QCheckBox( "Sequence");
    sequence_check->setChecked( was_sequence);

    layout->addWidget( relative_check);
    layout->addWidget( sequence_check);

    QGridLayout *glayout = (QGridLayout *) dialog.layout();
    glayout->addWidget( extra_widgets, glayout->rowCount(), 0, 1, glayout->columnCount(), Qt::AlignLeft);

    dialog.show();

    QStringList filenames;

    if( dialog.exec())
    {
        filenames = dialog.selectedFiles();
        p = filenames[0].toStdString();

        relative = relative_check->isChecked();
        was_relative = relative;

        if( relative_check->isChecked())
        {
            boost::filesystem::path dir = app().document().file().parent_path();
            p = filesystem::make_relative_path( p, dir);
        }

        sequence = sequence_check->isChecked();
        was_sequence = sequence;
        return true;
    }

    return false;
}

// serialization
void read_ui_state( const serialization::yaml_iarchive_t& in)
{
	// TODO: implement this
}

void user_interface_t::write_ui_state( serialization::yaml_oarchive_t& out) const
{
	// out << YAML::BeginMap;
	// save state( out)
	// out << YAML::EndMap;
}

// event filter
void user_interface_t::start_long_process()
{ 
	RAMEN_ASSERT( !event_filter_installed_);
	
	cancelled_ = false;
	qApp->installEventFilter( this);
	event_filter_installed_ = true;
}

void user_interface_t::process_events() { qApp->processEvents();}

bool user_interface_t::process_cancelled() const	{ return cancelled_;}

void user_interface_t::end_long_process()
{ 
	RAMEN_ASSERT( event_filter_installed_);	
	
	qApp->removeEventFilter( this);
	event_filter_installed_ = false;
}

bool user_interface_t::eventFilter( QObject *watched, QEvent *event)
{
	switch( event->type())
	{
	case QEvent::KeyPress:
	{
		QKeyEvent *key_event = dynamic_cast<QKeyEvent*>( event);

		if( key_event->key() == Qt::Key_Escape)
			cancelled_ = true;

		return true;
	}
	break;
	
	case QEvent::KeyRelease:
	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
		return true;

	case QEvent::MouseMove:
	{
		QMouseEvent *mouse_event = dynamic_cast<QMouseEvent*>( event);
		return mouse_event->buttons() == Qt::NoButton;
	}
	break;
	
	default:
		return false; // pass all other events
	}
}

boost::unique_future<bool>& user_interface_t::render_image( render::context_t context, render::image_node_renderer_t& renderer)
{
	RAMEN_ASSERT( !rendering_);
	
	cancelled_ = false;
	context.mode = render::interface_render;
	context.cancel = boost::bind( &user_interface_t::process_cancelled, this);
	renderer.set_context( context);
	rendering_ = true;
    boost::unique_future<bool>& future( app().render_thread().render_image( renderer));

	if( future.is_ready())
	{
		rendering_ = false;
		return future;
	}

	/*
	start_long_process();

	while( !future.timed_wait( boost::posix_time::milliseconds( 30)))
		process_events();

	end_long_process();
	*/
	
	future.wait();
	rendering_ = false;
	return future;
}

int user_interface_t::viewer_toolbar_height() const
{
    return viewer().toolbar_height();
}

QFont user_interface_t::get_fixed_width_code_font()
{
	QFont font;
	font.setFamily( "Courier");
	font.setFixedPitch( true);
	font.setPointSize( 11);
	return font;
}

} // namespace
} // namespace
