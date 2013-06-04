// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_NODE_IMAGE_RASTERIZABLE2D_INTERFACE_HPP
#define	RAMEN_NODE_IMAGE_RASTERIZABLE2D_INTERFACE_HPP

#include<OpenEXR/ImathMatrix.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

class rasterizable2d_interface
{
public:

	typedef Imath::V2f	vector_type;
	typedef Imath::M33f matrix_type;
	
	rasterizable2d_interface();
	virtual ~rasterizable2d_interface();

private:
	
};

} // namespace
} // namespace

#endif
