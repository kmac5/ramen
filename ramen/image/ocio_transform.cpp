// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/ocio_transform.hpp>

#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<ramen/assert.hpp>

#include<ramen/image/denan.hpp>

namespace ramen
{
namespace image
{

namespace detail
{

struct ocio_transform_fun
{
    ocio_transform_fun( const image::image_view_t& img,
                        OCIO::ConstProcessorRcPtr proc) : img_( img), proc_( proc)
    {
    }

    void operator()( const tbb::blocked_range<int>& r) const
    {
        for( int j = r.begin(); j < r.end(); ++j)
        {
            image::image_view_t::x_iterator it( img_.row_begin( j));
			
			// Is it the best way to get a ptr to a scanline?
            OCIO::PackedImageDesc pixels( reinterpret_cast<float*>( &( (*it)[0])),
                                            img_.width(), 1, 4);
            proc_->apply( pixels);
        }
    }

private:

    const image::image_view_t& img_;
    OCIO::ConstProcessorRcPtr proc_;
};

} // detail

void ocio_transform( const image_view_t& img, OCIO::ConstProcessorRcPtr proc)
{
	RAMEN_ASSERT( proc);
	
    if( !proc->isNoOp())
        tbb::parallel_for( tbb::blocked_range<int>( 0, img.height()),
                           detail::ocio_transform_fun( img, proc),
                           tbb::auto_partitioner());
}

} // image
} // ramen
