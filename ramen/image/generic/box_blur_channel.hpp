// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_BOX_BLUR_CHANNEL_HPP
#define RAMEN_IMAGE_BOX_BLUR_CHANNEL_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void box_blur_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					   const channel_view_t& dst, float hradius, float vradius, int iters);

void box_blur_gray( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					const boost::gil::gray32f_view_t& dst, float hradius, float vradius, int iters);

} // namespace
} // namespace
} // namespace

#endif
