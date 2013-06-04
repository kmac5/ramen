// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_NODE_IMAGE_COLOR_OP_INTERFACE_HPP
#define	RAMEN_NODE_IMAGE_COLOR_OP_INTERFACE_HPP

#include<OpenEXR/ImathColor.h>

namespace ramen
{
namespace image
{

class color_op_interface
{
public:

	color_op_interface();
	virtual ~color_op_interface();
	
	virtual bool has_crosstalk() const { return false;}
	
	Imath::Color3f transform_color( const Imath::Color3f& c) const;
	
private:
	
	virtual Imath::Color3f do_transform_color( const Imath::Color3f& c) const = 0;
};

} // namespace
} // namespace

#endif
