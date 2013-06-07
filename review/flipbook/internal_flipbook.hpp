// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLIPBOOK_INTERNAL_FLIPBOOK_HPP
#define RAMEN_FLIPBOOK_INTERNAL_FLIPBOOK_HPP

#include<ramen/flipbook/flipbook.hpp>

#include<ramen/GL/glew.hpp>

#include<QGLWidget>

#include<memory.h>

#include<boost/ptr_container/ptr_vector.hpp>

#include<OpenEXR/ImfIO.h>
#include<OpenEXR/ImfHeader.h>

#include<ramen/ui/viewport.hpp>

#include<ramen/flipbook/Timer.h>
#include<ramen/flipbook/ImageBuffers.h>
#include<ramen/flipbook/FileReadingThread.h>
#include<ramen/imageio/imf_memory_stream.hpp>

namespace ramen
{
namespace flipbook
{

class internal_flipbook_t : public QGLWidget, public flipbook_t
{
    Q_OBJECT

    internal_flipbook_t( int frame_rate, 
						const std::string& display_device,
						const std::string& display_transform);
	
public:

	static internal_flipbook_t *create( int frame_rate, const std::string& display_device, const std::string& display_transform);
		
	~internal_flipbook_t();
	
	virtual void set_format( const Imath::Box2i& f, float aspect, int subsample);
	
    virtual bool empty() const { return frames_.empty();}

	virtual void add_frame( int frame, image::buffer_t pixels);
	
	virtual void play();

    std::size_t num_frames() const { return frames_.size();}
    ImageBuffers& image_buffers() { return ib_;}	
    imageio::imf_memory_istream stream_for_frame( int f);
	
protected:

    virtual void initializeGL();
    virtual void resizeGL( int w, int h);
    virtual void paintGL();
	
    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);
	
    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseMoveEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);
	
    virtual void closeEvent( QCloseEvent *event);
	
private:
	
    Imath::V2f screen_to_world( const Imath::V2i& p) const;
    Imath::V2i world_to_screen( const Imath::V2f& p) const;
	
	void read_exr( Imf::IStream& is, boost::gil::rgba16f_image_t& image) const;
	void write_exr( Imf::OStream& os, Imf::Header& header, const boost::gil::rgba16fc_view_t& view) const;

    void fatal_gl_error( const QString& message) const;
    void test_gl_extensions();
	
	void load_texture( const imageio::imf_memory_ostream& os);
	void update_texture();
	
    void draw_frame();
	
    bool first_time_;
    ui::viewport_t viewport_;
	
    ImageBuffers ib_;
    int buffer_index_;
	
    Timer timer_;
    FileReadingThread *frt_;

    boost::ptr_vector<imageio::imf_memory_ostream> frames_;
    std::vector<char *> pixels_;

    bool playing_;
    int framenumber_;
    GLuint texture_id_;
	
	boost::gil::rgba16f_image_t half_buffer_;
	std::size_t frame_mem_size_;
	
	bool use_aspect_;
	
	// event handling
	bool scroll_mode_;
	bool zoom_mode_;
	Imath::V2f zoom_center_;
	Imath::V2i last_pos_;
};

} // namespace
} // namespace

#endif
