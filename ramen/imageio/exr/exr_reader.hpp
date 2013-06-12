// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_EXR_READER_HPP
#define	RAMEN_IMAGEIO_EXR_READER_HPP

#include<ramen/imageio/multichannel_reader.hpp>

#include<OpenEXR/ImfHeader.h>

namespace ramen
{
namespace imageio
{

class exr_reader_t : public multichannel_reader_t
{
public:

    exr_reader_t( const boost::filesystem::path& p);

private:

    virtual void do_read_image( const image::image_view_t& view,
                                const math::box2i_t& crop,
                                int subsample,
                                const boost::tuple<int,int,int,int>& channels) const;

private:

    bool is_rgb() const;
    bool is_luma_chroma() const;

    void read_exr_image( const boost::filesystem::path& p,
                         const image::image_view_t& result_view,
                         const math::box2i_t& crop,
                         std::size_t subsample = 1) const;
	
    void read_exr_image( const boost::filesystem::path& p,
                         const image::image_view_t& result_view,
                         const math::box2i_t& crop,
                         const char *rchannel,
                         const char *gchannel,
                         const char *bchannel,
                         const char *achannel,
                         std::size_t subsample = 1) const;
	
    void read_exr_luma_chroma_image( const boost::filesystem::path& p,
                                     const image::image_view_t& result_view,
                                     const math::box2i_t& crop,
                                     std::size_t subsample = 1) const;
	
    Imf::Header header_;
    bool is_rgb_image_;
    bool is_luma_chroma_image_;
};
    
} // imageio
} // ramen

#endif
