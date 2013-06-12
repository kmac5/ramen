// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_OIIO_READER_HPP
#define	RAMEN_IMAGEIO_OIIO_READER_HPP

#include<ramen/imageio/reader.hpp>

#include<OpenImageIO/imageio.h>
#include<OpenImageIO/typedesc.h>

namespace ramen
{
namespace imageio
{

class oiio_reader_t : public reader_t
{
public:

    oiio_reader_t( const boost::filesystem::path& p);

private:

    virtual void do_read_image( const image::image_view_t& view,
                                const math::box2i_t& crop,
                                int subsample) const;

    void do_read_tiled_image( const image::image_view_t& view,
                              const math::box2i_t& crop,
                              int subsample) const;
	
    void copy_tile( int x,
                    int y,
                    const OIIO::ImageSpec& spec,
                    float *data,
                    const math::box2i_t& crop,
                    const image::image_view_t& view,
                    int subsample) const;
	
	bool is_tiled_;
};

} // imageio
} // ramen

#endif
