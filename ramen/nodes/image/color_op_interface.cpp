// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/color_op_interface.hpp>

namespace ramen
{
namespace image
{

color_op_interface::color_op_interface() {}
color_op_interface::~color_op_interface() {}

Imath::Color3f color_op_interface::transform_color( const Imath::Color3f& c) const { return do_transform_color( c);}
	
} // namespace
} // namespace
