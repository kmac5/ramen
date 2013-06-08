// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/buffer.hpp>

#include<stdint.h>

#include<iostream>

#include<boost/static_assert.hpp>

#include<ramen/assert.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/memory/manager.hpp>
#include<ramen/memory/util.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{
namespace image
{
namespace
{

struct array_deleter
{
    array_deleter( std::size_t size) : size_( size)
    {
        RAMEN_ASSERT( size_ != 0 && "Passing 0 size to array deleter");
    }

    void operator()( unsigned char *p) const
    {
        app().memory_manager().image_allocator().deallocate( p, size_);
    }

    std::size_t size_;
};

} // namespace

// by default, 16 bytes (SSE) alignment. Must be a power of two
const unsigned int buffer_t::alignment = 16;

BOOST_STATIC_ASSERT( (( buffer_t::alignment - 1) & buffer_t::alignment) == 0);

buffer_t::buffer_t() { init();}

buffer_t::buffer_t( int width, int height, int channels)
{
    RAMEN_ASSERT( ( channels == 1 || channels == 3 || channels == 4) && "buffer_t: only 1, 3 and 4 channels images supported");

	init();
	channels_ = channels;
    bounds_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width-1, height-1));

    if( bounds_.isEmpty())
		return;

    alloc_pixels();
}

buffer_t::buffer_t( const Imath::Box2i& bounds, int channels)
{
    RAMEN_ASSERT( ( channels == 1 || channels == 3 || channels == 4) && "buffer_t: only 1, 3 and 4 channels images supported");

	init();
	bounds_ = bounds;
	channels_ = channels;

    if( bounds_.isEmpty())
		return;

    alloc_pixels();
}

void buffer_t::init()
{
	cached_pixels_ = false;
	use_disk_cache_ = false;
	channels_ = 0;
	rowbytes_ = 0;
}

void buffer_t::alloc_pixels()
{
    std::size_t size = (( width() * height() * channels()) * sizeof( float)) + alignment;
    pixels_.reset( app().memory_manager().image_allocator().allocate( size), array_deleter( size));
	rowbytes_ = width() * channels() * sizeof( float);
}

unsigned char *buffer_t::aligned_ptr() const
{
	return memory::aligned_ptr( pixels_.get(), alignment);
}

int buffer_t::channels() const { return channels_;}

void buffer_t::clear()
{
    cached_pixels_ = false;
    bounds_ = Imath::Box2i();
    pixels_.reset();
}

const_image_view_t buffer_t::const_rgba_view() const
{
    RAMEN_ASSERT( !empty() && "Trying to get a view from an empty image");
    RAMEN_ASSERT( channels() == 4);

    return boost::gil::interleaved_view<const_pixel_ptr_t>( width(), height(), 
															reinterpret_cast<const_pixel_ptr_t>( aligned_ptr()),
                                                            width() * channels() * sizeof( float));
}

const_rgb_image_view_t buffer_t::const_rgb_view() const
{
    RAMEN_ASSERT( !empty() && "Trying to get a view from an empty image");
    RAMEN_ASSERT( channels() == 3);

    return boost::gil::interleaved_view<const_rgb_pixel_ptr_t>( width(), height(), 
																reinterpret_cast<const_rgb_pixel_ptr_t>( aligned_ptr()),
														          width() * channels() * sizeof( float));
}

const_gray_image_view_t buffer_t::const_gray_view() const
{
    RAMEN_ASSERT( !empty() && "Trying to get a view from an empty image");
    RAMEN_ASSERT( channels() == 1);

    return boost::gil::interleaved_view<const_gray_pixel_ptr_t>( width(), height(), 
																 reinterpret_cast<const_gray_pixel_ptr_t>( aligned_ptr()),
                                                                width() * channels() * sizeof( float));
}

const_image_view_t buffer_t::const_rgba_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( const_rgba_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
										area.size().x+1, area.size().y+1);
}

const_rgb_image_view_t buffer_t::const_rgb_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( const_rgb_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
										area.size().x+1, area.size().y+1);
}

const_gray_image_view_t buffer_t::const_gray_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( const_gray_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
										area.size().x+1, area.size().y+1);
}

image_view_t buffer_t::rgba_view() const
{
    RAMEN_ASSERT( !empty()  && "Trying to get a view of an empty image");
    RAMEN_ASSERT( !cached() && "Trying to get a non const view of an image cached");
    RAMEN_ASSERT( channels() == 4);

    return boost::gil::interleaved_view<pixel_ptr_t>( width(), height(), 
													  reinterpret_cast<pixel_ptr_t>( aligned_ptr()),
                                                        width() * channels() * sizeof( float));

}

rgb_image_view_t buffer_t::rgb_view() const
{
    RAMEN_ASSERT( !empty()  && "Trying to get a view of an empty image");
    RAMEN_ASSERT( !cached() && "Trying to get a non const view of an image cached");
    RAMEN_ASSERT( channels() == 3);

    return boost::gil::interleaved_view<rgb_pixel_ptr_t>( width(), height(), 
														  reinterpret_cast<rgb_pixel_ptr_t>( aligned_ptr()),
												            width() * channels() * sizeof( float));
}

gray_image_view_t buffer_t::gray_view() const
{
    RAMEN_ASSERT( !empty()  && "Trying to get a view of an empty image");
    RAMEN_ASSERT( !cached() && "Trying to get a non const view of an image cached");
    RAMEN_ASSERT( channels() == 1);
    
    return boost::gil::interleaved_view<gray_pixel_ptr_t>( width(), height(), 
														   reinterpret_cast<gray_pixel_ptr_t>( aligned_ptr()),
                                                            width() * channels() * sizeof( float));
}

image_view_t buffer_t::rgba_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( rgba_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
										area.size().x+1, area.size().y+1);
}

rgb_image_view_t buffer_t::rgb_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( rgb_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
										area.size().x+1, area.size().y+1);
}

gray_image_view_t buffer_t::gray_subimage_view( const Imath::Box2i& area) const
{
    check_area_inside_image( area);
    return boost::gil::subimage_view( gray_view(), area.min.x - bounds_.min.x, area.min.y - bounds_.min.y,
										area.size().x+1, area.size().y+1);
}

void buffer_t::check_area_inside_image( const Imath::Box2i& area) const
{
	#ifndef NDEBUG
        if( !ImathExt::isInside( bounds_, area))
		{
		    // Not OK, print some debug info and exit
		    std::cout << "Trying to access pixels outside buffer\n";
            //std::cout << "Area = " << area << " Bounds = " << bounds_ << "\n";
			RAMEN_ASSERT( 0);
		}
	#endif
}

bool buffer_t::operator==( const buffer_t& other) const
{
    return ( bounds() == other.bounds()) && ( pixels_ == other.pixels_);
}

bool buffer_t::operator!=( const buffer_t& other) const
{
    return ( bounds() != other.bounds()) || ( pixels_ != other.pixels_);
}

} // image
} // ramen
