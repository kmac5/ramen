// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATH_GL_HPP
#define	RAMEN_IMATH_GL_HPP

#include<ramen/GL/gl.hpp>

#include<OpenEXR/ImathVec.h>
#include<OpenEXR/ImathMatrix.h>
#include<OpenEXR/ImathColor.h>

namespace ramen
{

void gl_vertex( const Imath::V2i& p);
void gl_vertex( const Imath::V2f& p);
	
void gl_color( const Imath::Color3c& c);
void gl_color( const Imath::Color3f& c);
void gl_color( const Imath::Color4f& c);

void gl_mult_matrix( const Imath::M33f& m);
void gl_load_matrix( const Imath::M33f& m);

void gl_mult_matrix( const Imath::M44f& m);
void gl_load_matrix( const Imath::M44f& m);
	
} // ramen

#endif
