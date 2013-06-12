// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOVIEIO_WRITER_HPP
#define	RAMEN_MOVIEIO_WRITER_HPP

#include<ramen/config.hpp>

#include<ramen/core/dictionary.hpp>
#include<ramen/filesystem/path.hpp>
#include<ramen/image/typedefs.hpp>

#include<ramen/movieio/exceptions.hpp>

namespace ramen
{
namespace movieio
{

class RAMEN_API writer_t
{
public:

    writer_t();
    virtual ~writer_t();
	
    void write_frame( const image::const_image_view_t& view,
                        const core::dictionary_t& params) const;

private:

    // non-copyable
    writer_t( const writer_t&);
    writer_t& operator=( const writer_t&);

    virtual void do_write_frame( const image::const_image_view_t& view,
                                const core::dictionary_t& params) const = 0;
	
	boost::filesystem::path p_;
    core::dictionary_t metadata_;
};

} // movieio
} // ramen

#endif
