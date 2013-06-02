// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_GENERIC_WARP_HPP
#define RAMEN_IMAGE_GENERIC_WARP_HPP

#include<boost/function.hpp>

namespace ramen
{
namespace image
{

typedef boost::function<Imath::V2f ( const Imath::V2f&)> warp_function_t;

} // namespace
} // namespace

#endif
