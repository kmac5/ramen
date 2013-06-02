// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace
{
	
Imath::M44f convert_to_M44( const Imath::M33f& m)
{
	RAMEN_ASSERT( 0 && "Implement matrix33 to matrix44 for OpenGL");
	Imath::M44f result;
	return result;
}
	
} // unnamed
	
void gl_vertex( const Imath::V2i& p) { gl_vertex2i( p.x, p.y);}
void gl_vertex( const Imath::V2f& p) { gl_vertex2f( p.x, p.y);}

void gl_color( const Imath::Color3c& c) { gl_color3ub( c.x, c.y, c.z);}

void gl_color( const Imath::Color3f& c) { gl_color3f( c.x, c.y, c.z);}
void gl_color( const Imath::Color4f& c) { gl_color4f( c.r, c.g, c.b, c.a);}

void gl_mult_matrix( const Imath::M44f& m)
{ 
	glMultMatrixf( (GLfloat*) m[0]);
	check_gl_errors();
}

void gl_load_matrix( const Imath::M44f& m)
{ 
	glLoadMatrixf( (GLfloat*) m[0]);
	check_gl_errors();
}

void gl_mult_matrix( const Imath::M33f& m)
{
	gl_mult_matrix( convert_to_M44( m));
}

void gl_load_matrix( const Imath::M33f& m)
{
	gl_load_matrix( convert_to_M44( m));
}

} // ramen


/*
    S a, b, c, w;

    a = X * x[0][0] + Y * x[1][0] + src[2] * x[2][0] + x[3][0];
    b = X * x[0][1] + Y * x[1][1] + src[2] * x[2][1] + x[3][1];
    c = X * x[0][2] + Y * x[1][2] + src[2] * x[2][2] + x[3][2];
    w = X * x[0][3] + Y * x[1][3] + src[2] * x[2][3] + x[3][3];

    dst.x = a / w;
    dst.y = b / w;
    dst.z = c / w;

    S a, b, w;

    a = X * x[0][0] + Y * x[1][0] + x[2][0];
    b = X * x[0][1] + Y * x[1][1] + x[2][1];
    w = X * x[0][2] + Y * x[1][2] + x[2][2];

    dst.x = a / w;
    dst.y = b / w;

void convert( )
{
	Imath::M44f m44;
	Imath::M33f m33;
	
	m44[0][0] = m33[0][0];
	m44[1][0] = m33[1][0];
	m44[2][0] = 0;
	m44[3][0] = m33[2][0];

	m44[0][1] = m33[0][1];
	m44[1][1] = m33[1][1];
	m44[2][1] = 0;
	m44[3][1] = m33[2][1];

	m44[0][2] = 0;
	m44[1][2] = 0;
	m44[2][2] = 0;
	m44[3][2] = 0;

	m44[0][3] = m33[0][2];
	m44[1][3] = m33[1][2];
	m44[2][3] = 0;
	m44[3][3] = m33[2][2];
}
 
 Xa00 + Ya10 + a30 = Xb00 + Yb10 + b20;
 Xa01 + Ya11 + a31 = Xb01 + Yb11 + b21;
 Xa03 + Ya13 + a33 = Xb02 + Yb12 + b22;


 a30 = b20;
 a31 = b21;
 a33 = b22;
 */
