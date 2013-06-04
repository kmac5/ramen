// Copyright (c) 2010 Esteban Tovagliari.

#ifndef RAMEN_IMAGEIO_CINEON_READER_HPP
#define	RAMEN_IMAGEIO_CINEON_READER_HPP

#include<ramen/imageio/reader.hpp>

#include<boost/filesystem/fstream.hpp>

namespace ramen
{
namespace imageio
{

class cineon_reader_t : public reader_t
{
public:

    cineon_reader_t( const boost::filesystem::path& p);

private:

    virtual void do_read_image( const image::image_view_t& view, const Imath::Box2i& crop, int subsample) const;

    void read_header( boost::filesystem::ifstream& in) const;

    mutable int width_, height_;
    mutable boost::uint32_t data_offset_;
    mutable bool reverse_bytes_;
};

} // namespace
} // namespace

#endif
