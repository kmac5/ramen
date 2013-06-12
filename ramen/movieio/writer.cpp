// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/movieio/writer.hpp>

namespace ramen
{
namespace movieio
{

writer_t::writer_t() {}
writer_t::~writer_t() {}

void writer_t::write_frame( const image::const_image_view_t& view,
                            const core::dictionary_t& params) const
{
	do_write_frame( view, params);
}

} // movieio
} // ramen
