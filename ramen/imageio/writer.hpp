// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_WRITER_HPP
#define	RAMEN_IMAGEIO_WRITER_HPP

#include<ramen/config.hpp>

#include<vector>

#include<boost/cstdint.hpp>

#include<adobe/dictionary.hpp>
#include<adobe/algorithm/clamp.hpp>

#include<ramen/assert.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/math/fast_float.hpp>

#include<ramen/image/typedefs.hpp>

#include<ramen/imageio/exceptions.hpp>
#include<ramen/imageio/enums.hpp>

namespace ramen
{
namespace imageio
{

class RAMEN_API writer_t
{
public:

    writer_t() {}
    virtual ~writer_t() {}

    void write_image( const boost::filesystem::path& p,
						const image::const_image_view_t& view,
						const adobe::dictionary_t& params) const
    {
        do_write_image( p, view, params);
    }

protected:

    boost::uint8_t convert8( float x) const
    {
       return math::fast_float_to_int( adobe::clamp( x, 0.0f, 1.0f) * 255.0);
    }

    boost::uint16_t convert16( float x) const
    {
       return math::fast_float_to_int( adobe::clamp( x, 0.0f, 1.0f) * 65535.0);
    }

    template<class PixelIter>
    void clamp( PixelIter start, PixelIter end) const
    {
        for( PixelIter it( start); it != end; ++it)
        {
            boost::gil::get_color( *it, boost::gil::red_t())   = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::red_t()), 0.0f, 1.0f);
            boost::gil::get_color( *it, boost::gil::green_t()) = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::green_t()), 0.0f, 1.0f);
            boost::gil::get_color( *it, boost::gil::blue_t())  = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::blue_t()), 0.0f, 1.0f);
            boost::gil::get_color( *it, boost::gil::alpha_t()) = adobe::clamp( (float) boost::gil::get_color( *it, boost::gil::alpha_t()), 0.0f, 1.0f);
        }
    }

private:

    virtual void do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const = 0;
};

} // namespace
} // namespace

#endif
