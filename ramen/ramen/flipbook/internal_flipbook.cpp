// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/flipbook/internal_flipbook.hpp>

#include<iostream>

#include<QApplication>
#include<QKeyEvent>
#include<QMouseEvent>
#include<QCloseEvent>
#include<QMessageBox>
#include<QDesktopWidget>

#include<OpenEXR/half.h>
#include<OpenEXR/ImfInputFile.h>
#include<OpenEXR/ImfOutputFile.h>
#include<OpenEXR/ImfChannelList.h>

#include<ramen/assert.hpp>

#include<ramen/GL/glu.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{

namespace flipbook
{

internal_flipbook_t::internal_flipbook_t( int frame_rate, 
										const std::string& display_device,
										const std::string& display_transform) : QGLWidget(), flipbook_t( frame_rate, display_device, display_transform)
{
	setFocusPolicy( Qt::ClickFocus);
	setWindowTitle( "Flipbook");
	setMouseTracking( true);

	frt_ = 0;
	playing_ = false;
	first_time_ = true;
	buffer_index_ = 0;	
	framenumber_ = 0;	
    timer_.setDesiredFrameRate( frame_rate);
	frame_mem_size_ = 0;
	
	scroll_mode_ = zoom_mode_ = false;
	use_aspect_ = true;
}

internal_flipbook_t *internal_flipbook_t::create( int frame_rate, const std::string& display_device, const std::string& display_transform)
{
	return new internal_flipbook_t( frame_rate, display_device, display_transform);
}

internal_flipbook_t::~internal_flipbook_t()
{
    makeCurrent();

	gl_delete_texture( &texture_id_);

    for( int i = 0; i < pixels_.size(); ++i)
		delete[] pixels_[i];

    delete frt_;
}

void internal_flipbook_t::set_format( const Imath::Box2i& f, float aspect, int subsample)
{
	flipbook_t::set_format( f, aspect, subsample);
	
    int w = image_width();
    int h = image_height();
	
	half_buffer_ = boost::gil::rgba16f_image_t( w, h);

	// get image size in bytes
	{
		imageio::imf_null_ostream os;
		Imf::Header header = Imf::Header( w, h);
		header.compression() = Imf::B44_COMPRESSION;
		write_exr( os, header, boost::gil::const_view( half_buffer_));
		frame_mem_size_ = os.size();
	}
	
    // initialize frame buffers
    ib_.rgbMode = true;
    ib_.dataWindow = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( w - 1, h - 1));

    for (int i = 0; i < ib_.numBuffers(); ++i)
    {
		Imf::FrameBuffer& fb = ib_.frameBuffer( i);
	
		size_t pixelSize = sizeof( half) * 4;
		size_t lineSize = pixelSize * w;
	
		char *pixels = new char[lineSize * h];
		pixels_.push_back( pixels);
	
		ib_.pixels( i, 0) = pixels;
		ib_.pixels( i, 1) = pixels + sizeof( half);
		ib_.pixels( i, 2) = pixels + sizeof( half) * 2;
	
		fb.insert( "R", Imf::Slice( Imf::HALF, ib_.pixels( i, 0), pixelSize, lineSize, 1, 1));
		fb.insert( "G", Imf::Slice( Imf::HALF, ib_.pixels( i, 1), pixelSize, lineSize, 1, 1));
		fb.insert( "B", Imf::Slice( Imf::HALF, ib_.pixels( i, 2), pixelSize, lineSize, 1, 1));
    }

    // initialize texture
    makeCurrent();

    gl_pixel_storei( GL_UNPACK_ALIGNMENT, 1);
    texture_id_ = gl_gen_texture();

    gl_bind_texture( GL_TEXTURE_2D, texture_id_);
    gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_tex_envf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    gl_tex_image2d( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, w, h, 0, GL_RGBA, GL_HALF_FLOAT_ARB, 0);

	// adjust the window size to take the aspect into account
	w *= aspect;
	
	// clamp the window size to the Desktop.
	QDesktopWidget *desktop = QApplication::desktop();
	QRect screen_area = desktop->availableGeometry();	
	int window_width = std::min( w, ( int) screen_area.width() - 20);
	int window_height = std::min( h, ( int) screen_area.height() - 20);
    resize( window_width, window_height);
}

void internal_flipbook_t::add_frame( int frame, image::buffer_t pixels)
{
	flipbook_t::add_frame( frame, pixels);
	boost::gil::copy_and_convert_pixels( const_view(), boost::gil::view( half_buffer_));
	std::auto_ptr<imageio::imf_memory_ostream> os( new imageio::imf_memory_ostream( frame_mem_size_));

	if( os.get())
	{
		Imf::Header header = Imf::Header( image_width(), image_height());
		header.compression() = Imf::B44_COMPRESSION;
		write_exr( *os, header, boost::gil::const_view( half_buffer_));
		frames_.push_back( os);
		
		if( frames_.size() == 1)
		{
			Imath::V2f p( image_width() / 2, image_height() / 2);
			viewport_.scroll_to_center_point( p);
			show();
		}
		
		load_texture( frames_.back());
		setWindowTitle( QString( "Frame %1").arg( frame + 1));
		update();
		qApp->processEvents();		
	}
	else
		throw std::bad_alloc(); // out of memory...
}

void internal_flipbook_t::play()
{
	flipbook_t::play();
	half_buffer_ = boost::gil::rgba16f_image_t();	
    playing_ = true;
    update();
}

imageio::imf_memory_istream internal_flipbook_t::stream_for_frame( int f)
{
    RAMEN_ASSERT( f >= 0);
    RAMEN_ASSERT( f < num_frames());
    return imageio::imf_memory_istream( frames_[f].data(), frames_[f].size());
}

void internal_flipbook_t::read_exr( Imf::IStream& is, boost::gil::rgba16f_image_t& image) const
{
    Imf::InputFile file( is);
    Imath::Box2i dw( file.header().dataWindow());

    int width  = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    if( ( image.width() != width) || ( image.height() != height))
	image.recreate( width, height);

    boost::gil::rgba16f_view_t view( boost::gil::view( image));

    std::size_t xstride = 4 * sizeof( half);
    std::size_t ystride = width * xstride;

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);

    Imf::FrameBuffer frameBuffer;

    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr , xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr , xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr , xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "A", Imf::Slice( Imf::HALF, ptr , xstride, ystride, 1, 1, 1.0));

    file.setFrameBuffer( frameBuffer);
    file.readPixels( dw.min.y, dw.max.y);
}

void internal_flipbook_t::write_exr( Imf::OStream& os, Imf::Header& header, const boost::gil::rgba16fc_view_t& view) const
{
	RAMEN_ASSERT( header.compression() == Imf::B44_COMPRESSION);

    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    std::size_t xstride = 4 * sizeof( half);
    std::size_t ystride = xstride * view.width();

    header.channels().insert( "R", Imf::HALF);
    header.channels().insert( "G", Imf::HALF);
    header.channels().insert( "B", Imf::HALF);

    Imf::FrameBuffer frameBuffer;
    frameBuffer.insert( "R", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "G", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof( half);
    frameBuffer.insert( "B", Imf::Slice( Imf::HALF, ptr, xstride, ystride)); ptr += sizeof( half);

    Imf::OutputFile out_file( os, header);
    out_file.setFrameBuffer( frameBuffer);
    out_file.writePixels( view.height());
}

void internal_flipbook_t::fatal_gl_error( const QString& message) const
{
    QMessageBox::warning( (QWidget *) app().ui()->main_window(),
			 "OpenGL error", message);
    exit( 0);
}

void internal_flipbook_t::test_gl_extensions()
{
    GLenum err = glewInit();

    if( GLEW_OK != err)
		fatal_gl_error( "Video card not supported: Error initializing GLEW");

    if( !glewIsSupported("GL_VERSION_2_0"))
        fatal_gl_error( "Video card not supported: Not supported OpenGL 2.0");

    if( !GLEW_ARB_texture_non_power_of_two)
        fatal_gl_error( "Video card not supported: Not supported non power of two textures");

    if( !GLEW_ARB_texture_float)
        fatal_gl_error( "Video card not supported: Not supported float textures");

    // Todo: Test Half textures here
}

void internal_flipbook_t::initializeGL()
{
    if( first_time_)
    {
		test_gl_extensions();
		viewport_.reset( width(), height());
		first_time_ = false;
    }

    gl_clear_color( 0, 0, 0, 0);
}

void internal_flipbook_t::resizeGL( int w, int h) { viewport_.resize( w, h);}

void internal_flipbook_t::paintGL()
{	
    if( !playing_)
    {
		draw_frame();
		gl_flush();
		return;
    }

    if( !frt_)
    {
		frt_ = new FileReadingThread( this);
		timer_.playState = ( num_frames() != 1) ? RUNNING: PREPARE_TO_PAUSE;
    }

    if( ib_.exitSemaphore2.tryWait())
		close();

    // Wait until it is time to display the next image
    timer_.waitUntilNextFrameIsDue();

    // Wait until the file reading thread has made the next frame available
    if( timer_.playState == RUNNING || timer_.playState == PREPARE_TO_PAUSE)
		ib_.fullBuffersSemaphore.wait();

    if( timer_.playState == PREPARE_TO_PAUSE)
		timer_.playState = PAUSE;

    framenumber_ = ib_.frameNumber( buffer_index_);
	setWindowTitle( QString( "Frame %1").arg( framenumber_ + 1));
    update_texture();
    draw_frame();

    // Return the image buffer to the file reading thread
    if (timer_.playState == RUNNING || timer_.playState == PREPARE_TO_RUN)
    {
		buffer_index_ = (buffer_index_ + 1) % ib_.numBuffers();
		ib_.emptyBuffersSemaphore.post();
    }

    if( timer_.playState == PREPARE_TO_RUN)
		timer_.playState = RUNNING;

    gl_flush();

    // Make sure this function gets called again immediately
    if( timer_.playState == RUNNING || timer_.playState == PREPARE_TO_RUN)
		update();	
}

void internal_flipbook_t::keyPressEvent( QKeyEvent *event)
{
    switch( event->key())
    {
	    case Qt::Key_Escape:
			close();
			event->accept();
	    break;

		case Qt::Key_Home:
		{
			viewport_.reset();
			Imath::V2f p( image_width() * aspect_ratio() / 2, image_height() / 2);
			viewport_.scroll_to_center_point( p);
			update();
			event->accept();
		}
		break;
	
		case Qt::Key_Comma:
		{
			Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
			viewport_.zoom( p, 2.0f);
			update();		
			event->accept();
		}
		break;
	
		case Qt::Key_Period:
		{
			Imath::V2f p( viewport_.screen_to_world( viewport_.device().center()));
			viewport_.zoom( p, 0.5f);
			update();
			event->accept();
		}
		break;
	
		case Qt::Key_A:
			use_aspect_ = !use_aspect_;
			update();
		break;
		
		case Qt::Key_P: // pause, loop forward
		case Qt::Key_L:
			if( playing_)
			{
				if( timer_.playState == RUNNING && ib_.forward)
					timer_.playState = PREPARE_TO_PAUSE;
	
				if( timer_.playState == PAUSE)
					timer_.playState = PREPARE_TO_RUN;
	
				ib_.forward = true;
				update();
				event->accept();
			}
		break;

		case Qt::Key_H: // pause, loop backward
			if( playing_)
			{
				if( timer_.playState == RUNNING && !ib_.forward)
					timer_.playState = PREPARE_TO_PAUSE;
		
				if( timer_.playState == PAUSE)
					timer_.playState = PREPARE_TO_RUN;
		
				ib_.forward = false;
				update();
				event->accept();
			}
		break;

		case Qt::Key_J:
		case Qt::Key_K:
			if( playing_)
			{
				// Step one frame forward or backward
				if( timer_.playState == RUNNING || timer_.playState == PREPARE_TO_PAUSE)
					ib_.fullBuffersSemaphore.wait();
		
				if( event->key() == Qt::Key_K)
					ib_.forward = true;
				else
					ib_.forward = false;
		
				timer_.playState = PAUSE;
		
				int new_framenumber;
		
				if( ib_.forward)
				{
					if( framenumber_ >= num_frames() - 1)
						new_framenumber = 0;
					else
						new_framenumber = framenumber_ + 1;
				}
				else
				{
					if( ib_.frameNumber( buffer_index_) <= 0)
						new_framenumber = num_frames() - 1;
					else
						new_framenumber = framenumber_ - 1;
				}
		
				while( ib_.frameNumber( buffer_index_) != new_framenumber)
				{
					buffer_index_ = ( buffer_index_ + 1) % ib_.numBuffers();
					ib_.emptyBuffersSemaphore.post();
					ib_.fullBuffersSemaphore.wait();
				}
		
				update();
				event->accept();
			}
		break;

		default:
		    event->ignore();
    }
}

void internal_flipbook_t::keyReleaseEvent( QKeyEvent *event)
{
    switch( event->key())
    {
	    case Qt::Key_Escape:
		case Qt::Key_Home:
		case Qt::Key_Comma:
		case Qt::Key_Period:
		case Qt::Key_A:
			event->accept();
	    break;

		case Qt::Key_P:
		case Qt::Key_L:
		case Qt::Key_H:
		case Qt::Key_J:
		case Qt::Key_K:
			if( playing_)
				event->accept();
			else
				event->ignore();
		break;
		
		default:
			event->ignore();
	}
}

void internal_flipbook_t::mousePressEvent( QMouseEvent *event)
{
	if(  event->button() == Qt::LeftButton)
	{
		scroll_mode_ = false;
		zoom_mode_ = false;

		last_pos_ = Imath::V2i( event->x(), event->y());
		
		if( event->modifiers() & Qt::AltModifier)
		{
			if( event->modifiers() & Qt::ShiftModifier)
			{
				zoom_mode_ = true;
				zoom_center_ = screen_to_world( last_pos_);
			}
			else
				scroll_mode_ = true;
			
			event->accept();
			return;
		}		
	}
	
	event->ignore();
}

void internal_flipbook_t::mouseMoveEvent( QMouseEvent *event)
{
	if( scroll_mode_ || zoom_mode_)
	{
		Imath::V2i pos( event->x(), event->y());
		
		if( ( pos.x != last_pos_.x) || ( pos.y != last_pos_.y))
		{
			if( scroll_mode_)
				viewport_.scroll( -( pos - last_pos_));
			else
			{
				const float zoom_speed = 0.05f;
				float zoom = 1.0f + ( zoom_speed * ( pos.x - last_pos_.x));
				viewport_.zoom( zoom_center_, zoom, zoom);
			}
	
			last_pos_ = pos;
		}
		
		event->accept();
		update();
	}
	else
		event->ignore();
}

void internal_flipbook_t::mouseReleaseEvent( QMouseEvent *event)
{
	scroll_mode_ = false;
	zoom_mode_ = false;
	event->accept();
}

void internal_flipbook_t::closeEvent( QCloseEvent *event)
{
    if( playing_)
    {
		ib_.exitSemaphore1.post();
		ib_.emptyBuffersSemaphore.post();
		ib_.exitSemaphore2.wait();
    }

    hide();
    cancelled();
    deleteLater();
    event->accept();
}

Imath::V2f internal_flipbook_t::screen_to_world( const Imath::V2i& p) const		{ return viewport_.screen_to_world( p);}
Imath::V2i internal_flipbook_t::world_to_screen( const Imath::V2f& p) const		{ return viewport_.world_to_screen( p);}

void internal_flipbook_t::load_texture( const imageio::imf_memory_ostream& os)
{
    makeCurrent();
    
    imageio::imf_memory_istream is( os.data(), os.size());
    read_exr( is, half_buffer_);
    boost::gil::rgba16f_view_t view( boost::gil::view( half_buffer_));

    // load in texture
	gl_enable( GL_TEXTURE_2D);
    gl_bind_texture( GL_TEXTURE_2D, texture_id_);
    char *ptr = (char *) boost::gil::interleaved_view_get_raw_data( view);
    gl_tex_image2d( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, image_width(), image_height(), 0, GL_RGBA, GL_HALF_FLOAT_ARB, ptr);
	gl_disable( GL_TEXTURE_2D);
}

void internal_flipbook_t::update_texture()
{
	gl_enable( GL_TEXTURE_2D);
    gl_bind_texture( GL_TEXTURE_2D, texture_id_);
    gl_tex_subimage2d( GL_TEXTURE_2D, 0, 0, 0, image_width(), image_height(), GL_RGBA, GL_HALF_FLOAT_ARB, (GLvoid *) ib_.pixels( buffer_index_, 0));
	gl_disable( GL_TEXTURE_2D);
}

void internal_flipbook_t::draw_frame()
{
	gl_clear( GL_COLOR_BUFFER_BIT);

    gl_matrix_mode( GL_MODELVIEW);
    gl_load_identity();

    gl_matrix_mode( GL_PROJECTION);
    gl_load_identity();
    gl_viewport( 0, 0, width(), height());
    glu_ortho2d( viewport_.world().min.x, viewport_.world().max.x, viewport_.world().max.y, viewport_.world().min.y);
	
    gl_matrix_mode( GL_MODELVIEW);
	
	if( use_aspect_)
		gl_scalef( aspect_ratio(), 1.0f);
	
    if( texture_id_)
    {
		gl_enable( GL_TEXTURE_2D);
		gl_bind_texture( GL_TEXTURE_2D, texture_id_);
		gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		gl_tex_parameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		gl_tex_envf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		gl_color4f( 0.0, 0.0, 0.0, 1.0);
		gl_textured_quad( 0, 0, image_width(), image_height(), true);
		gl_disable( GL_TEXTURE_2D);

		// draw red frame around image.
		gl_line_width( 1);
		gl_color4f( 0.75, 0.0, 0.0, 1.0);

		gl_begin( GL_LINE_LOOP);
			gl_vertex2i( -1, -1);
			gl_vertex2i( image_width() + 1, -1);
			gl_vertex2i( image_width() + 1, image_height() + 1);
			gl_vertex2i( -1, image_height() + 1);
		gl_end();
	}
}

} // namespace
} // namespace
