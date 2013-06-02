// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/flipbook/external_flipbook.hpp>

#include<boost/filesystem/operations.hpp>
#include<boost/filesystem/convenience.hpp>

#include<ramen/app/application.hpp>

#include<ramen/flipbook/factory.hpp>

#include<ramen/imageio/factory.hpp>
#include<ramen/imageio/enums.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{
namespace flipbook
{
	
external_flipbook_t::external_flipbook_t( int frame_rate, 
										  const std::string& display_device,
										 const std::string& display_transform) : flipbook_t( frame_rate, display_device, display_transform)
{
	empty_ = true;
	proc_ = new QProcess( this);
	progress_ = 0;

	boost::filesystem::path tmp_flip_dir = factory_t::instance().flipbooks_dir();
	int num = 1;
	
	while( 1)
	{
		std::string tmp = "flip" + boost::lexical_cast<std::string>( num);
		boost::filesystem::path p = tmp_flip_dir / tmp;
		
		if( !boost::filesystem::exists( p))
		{
			dir_ = p;
			boost::filesystem::create_directories( dir_);
			break;
		}
		
		++num;
	}

	fname_ = "img.";
	ext_ = ".exr";
	use_data_window_ = false;
}

external_flipbook_t::~external_flipbook_t() {}

void external_flipbook_t::begin_progress()
{
	progress_ = new QProgressDialog( "Rendering Flipbook", "Cancel", 0, 
									 end_ - start_ + 1, 
									 (QWidget *) app().ui()->main_window());
	
	progress_->setWindowModality( Qt::ApplicationModal);
	progress_->setWindowTitle( "Rendering Flipbook");
	progress_->show();
}

void external_flipbook_t::end_progress()
{
	if( progress_)
	{
		progress_->setValue( end_ - start_ + 1);
		progress_->hide();
		
		for( int i = 0; i < 5; ++i)
			app().ui()->process_events();
		
		progress_->deleteLater();
	}
}

void external_flipbook_t::add_frame( int frame, image::buffer_t pixels)
{
	for( int i = 0; i < 5; ++i)
		app().ui()->process_events();
	
	if( progress_->wasCanceled())
		cancelled();
	
	empty_ = false;
	flipbook_t::add_frame( frame, pixels);

	save_frame( frame);	
	
	progress_->setValue( frame - start_ + 1);
	
	for( int i = 0; i < 5; ++i)
		app().ui()->process_events();
}

void external_flipbook_t::save_frame( int frame) const
{
    boost::filesystem::path p( path_for_frame( frame));

    RAMEN_ASSERT( !p.empty());

    adobe::dictionary_t params;
    params[ adobe::name_t( "format")]	= adobe::any_regular_t( format_);
	params[ adobe::name_t( "aspect")]	= adobe::any_regular_t( aspect_);
	params[ adobe::name_t( "channels")]	= adobe::any_regular_t( (int) imageio::rgba_channels);
	params[ adobe::name_t( "type")]		= adobe::any_regular_t( (int) imageio::half_channel_type);
	params[ adobe::name_t( "compress")]	= adobe::any_regular_t( (int) imageio::b44_compression);

	if( use_data_window_)
	    params[ adobe::name_t( "bounds")]	= adobe::any_regular_t( defined_);
	
    std::auto_ptr<imageio::writer_t> writer( imageio::factory_t::instance().writer_for_tag( "exr"));

    if( writer.get())
        writer->write_image( p, buffer_.const_rgba_view(), params);
}

void external_flipbook_t::play()
{
	flipbook_t::play();
	
	std::vector<std::string> args;
	arguments( args);
	
	QStringList qargs;
	
	for( int i = 0; i < args.size(); ++i)
		qargs.append( QString::fromStdString( args[i]));
	
	connect( proc_, SIGNAL( finished( int)), this, SLOT( process_exited(int)));
	connect( proc_, SIGNAL( error( QProcess::ProcessError)), this, SLOT( process_error( QProcess::ProcessError)));
	proc_->start( QString::fromStdString( program()), qargs);
}

void external_flipbook_t::process_error( QProcess::ProcessError)
{
	app().ui()->error( "Error while launching flipbook");
	deleteLater();
}

void external_flipbook_t::process_exited( int status)
{
	cancelled();
	deleteLater();
}

boost::filesystem::path external_flipbook_t::path_for_frame( int num) const
{
	std::string fname = fname_;
	fname += boost::lexical_cast<std::string>( num);
	fname += ext_;
	return dir_ / fname;
}

} // namespace
} // namespace
