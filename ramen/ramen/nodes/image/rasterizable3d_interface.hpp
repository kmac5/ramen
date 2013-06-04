// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_NODE_IMAGE_RASTERIZABLE3D_INTERFACE_HPP
#define	RAMEN_NODE_IMAGE_RASTERIZABLE3D_INTERFACE_HPP

#include<OpenEXR/ImathMatrix.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

class rasterizable3d_interface
{
public:

	typedef Imath::V3f	vector_type;
	typedef Imath::M44f matrix_type;
	
	rasterizable3d_interface();
	virtual ~rasterizable3d_interface();

private:
	
};

} // namespace
} // namespace

#endif
