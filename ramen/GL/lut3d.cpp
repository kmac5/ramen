// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/GL/lut3d.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace gl
{

lut3d_t::lut3d_t( int lut_size, GLenum texture_unit) : texture_id_( 0), lut_size_( lut_size)
{
    texture_unit_ = texture_unit;
    data_.reset( new Imath::Color3f[ lut_size * lut_size * lut_size]);	
}

lut3d_t::~lut3d_t() { gl_delete_texture( &texture_id_);}

void lut3d_t::create_gl_texture()
{
	texture_id_ = gl_gen_texture();
    gl_bind_texture( GL_TEXTURE_3D, texture_id_);

	gl_pixel_storei( GL_UNPACK_ALIGNMENT, 1);
	gl_pixel_storei( GL_UNPACK_ROW_LENGTH, lut_size_);

	gl_tex_parameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl_tex_parameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl_tex_parameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl_tex_parameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_tex_parameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// allocate space for the texture
	gl_tex_image3d( GL_TEXTURE_3D, 0, GL_RGB32F, lut_size_, lut_size_, lut_size_, 0, GL_RGB, GL_FLOAT, 0);
    gl_bind_texture( GL_TEXTURE_3D, 0);
}

void lut3d_t::update_gl_texture()
{
    if( !texture_id_)
		create_gl_texture();

    gl_bind_texture( GL_TEXTURE_3D, texture_id_);

	#ifndef NDEBUG
		int size;
		glGetTexLevelParameteriv( GL_TEXTURE_3D, 0, GL_TEXTURE_WIDTH, &size);
		RAMEN_ASSERT( size == lut_size());
		glGetTexLevelParameteriv( GL_TEXTURE_3D, 0, GL_TEXTURE_HEIGHT, &size);
		RAMEN_ASSERT( size == lut_size());
		glGetTexLevelParameteriv( GL_TEXTURE_3D, 0, GL_TEXTURE_DEPTH, &size);
		RAMEN_ASSERT( size == lut_size());
	#endif

    gl_pixel_storei( GL_UNPACK_ALIGNMENT, 1);
    gl_pixel_storei( GL_UNPACK_ROW_LENGTH, lut_size_);

    gl_tex_subimage3d( GL_TEXTURE_3D, 0, 0, 0, 0, lut_size_, lut_size_, lut_size_, GL_RGB, GL_FLOAT, data());
    gl_bind_texture( GL_TEXTURE_3D, 0);
}

void lut3d_t::bind()
{
    gl_active_texture( texture_unit_);
    gl_bind_texture( GL_TEXTURE_3D, texture_id_);
}

void lut3d_t::unbind()
{
    gl_active_texture( texture_unit_);
    gl_bind_texture( GL_TEXTURE_3D, 0);
    gl_active_texture( GL_TEXTURE0);
}

} // namespace
} // namespace
