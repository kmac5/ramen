// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_DILATE_HPP
#define RAMEN_IMAGE_DILATE_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void dilate_channel( const const_channel_view_t& src, const channel_view_t& dst, float hradius, float vradius);
void dilate_channel( const const_channel_view_t& src, const boost::gil::gray32f_view_t& tmp, 
					 const channel_view_t& dst, float hradius, float vradius);

void dilate( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& dst, float hradius, float vradius);
void dilate( const boost::gil::gray32fc_view_t& src, const boost::gil::gray32f_view_t& tmp,
			 const boost::gil::gray32f_view_t& dst, float hradius, float vradius);

} // namespace
} // namespace

#endif
