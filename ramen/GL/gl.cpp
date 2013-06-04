// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/GL/gl.hpp>

#include<ramen/assert.hpp>

namespace ramen
{

bool inside_gl_begin_ = false;

#ifndef NDEBUG
	GLuint current_texture = 0;
	GLenum current_texture_unit = GL_TEXTURE0;
	int num_push_matrix = 0;
#endif

void clear_gl_errors()
{
	while( glGetError() != GL_NO_ERROR)
		;
}

void check_gl_errors()
{
	#ifndef NDEBUG
		GLint err = glGetError();

		if( err != GL_NO_ERROR)
		{
            /*
            DLOG( FATAL) << "OpenGL error: " << err << "\n" <<
                            "inside begin block = " << inside_begin_block() << "\n" <<
                            "current texture unit = " << current_texture_unit << "\n" <<
                            "current texture = " << current_texture << "\n" <<
                            "push / pop num = " << num_push_matrix << "\n";
            */
		}
	#endif
}

void gl_enable( GLenum cap)
{
	glEnable( cap);
	check_gl_errors();
}

void gl_disable( GLenum cap)
{
	glDisable( cap);
	check_gl_errors();
}

void gl_blend_func( GLenum sfactor, GLenum dfactor)
{
	glBlendFunc( sfactor, dfactor);
	check_gl_errors();
}

// programs
GLuint gl_create_program()
{
	GLuint result = glCreateProgram();
	check_gl_errors();
	return result;
}

void gl_delete_program( GLuint program)
{
	if( program != 0)
	{
		glDeleteProgram( program);
		check_gl_errors();
	}
}

void gl_link_program( GLuint program)
{
	glLinkProgram( program);
	check_gl_errors();
}

void gl_use_program( GLuint program)
{
	glUseProgram( program);
	check_gl_errors();
}

GLint gl_get_uniform_location( GLuint program, const GLchar *name)
{
	GLint result = glGetUniformLocation( program, name);
	check_gl_errors();
	return result;
}

void gl_uniform1i( GLint location, GLint v0)
{
	glUniform1i( location, v0);
	check_gl_errors();
}

void gl_uniform1f( GLint location, GLfloat v0)
{
	glUniform1f( location, v0);
	check_gl_errors();
}

void gl_uniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
	glUniform4f( location, v0, v1, v2, v3);
	check_gl_errors();
}

// shaders
GLhandleARB gl_create_shader( GLenum shader_type)
{
	GLhandleARB result = glCreateShader( shader_type);
	check_gl_errors();
	return result;
}

void gl_delete_shader( GLuint shader)
{
	if( shader)
	{
		glDeleteShader( shader);
		check_gl_errors();
	}
}

void gl_shader_source( GLuint shader, GLsizei count, const GLchar **string, const GLint *length)
{
	glShaderSource( shader, count, string, length);
	check_gl_errors();
}

void gl_compile_shader( GLuint shader)
{
	glCompileShader( shader);
	check_gl_errors();
}

void gl_attach_shader( GLuint program, GLuint shader)
{
	glAttachShader( program, shader);
	check_gl_errors();
}

void gl_detach_shader( GLuint program, GLuint shader)
{
	glDetachShader( program, shader);
	check_gl_errors();
}

// textures
GLuint gl_gen_texture()
{
	GLuint tex = 0;
	gl_gen_textures( 1, &tex);
	return tex;
}

void gl_gen_textures( GLsizei num, GLuint *tex)
{
	glGenTextures( num, tex);
	check_gl_errors();
}

void gl_delete_texture( GLuint *tex)
{
	if( *tex)
		gl_delete_textures( 1, tex);
}

void gl_delete_textures( GLsizei num, GLuint *tex)
{
	glDeleteTextures( num, tex);
	check_gl_errors();
}

void gl_bind_texture( GLenum target, GLuint texture)
{
	RAMEN_ASSERT( !inside_begin_block());
	glBindTexture( target, texture);
	check_gl_errors();

	#ifndef NDEBUG
		current_texture = texture;
	#endif
}

void gl_active_texture( GLenum texture)
{
	glActiveTexture( texture);
	check_gl_errors();

	#ifndef NDEBUG
		current_texture_unit = texture;
	#endif
}

void gl_tex_parameteri( GLenum target, GLenum pname, GLint param)
{
	glTexParameteri( target, pname, param);
	check_gl_errors();
}

void gl_tex_envf( GLenum target, GLenum pname, GLfloat param)
{
	glTexEnvf( target, pname, param);
	check_gl_errors();
}

void gl_tex_image2d( GLenum target, GLint level, GLint internal_format,
				 	GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *data)
{
	glTexImage2D( target, level, internal_format, width, height, border, format, type, data);
	check_gl_errors();
}

void gl_tex_subimage2d(	GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
						GLenum format, GLenum type, const GLvoid *data)
{
	glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, data);
	check_gl_errors();
}

void gl_tex_image3d( GLenum target, GLint level, GLint internal_format, GLsizei width,
					 GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *data)
{
	glTexImage3D( target, level, internal_format, width, height, depth, border, format, type, data);
	check_gl_errors();
}

void gl_tex_subimage3d(	GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset,
						GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *data)
{
	glTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
	check_gl_errors();
}

void gl_get_tex_image( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *img)
{
	glGetTexImage( target, level, format, type, img);
	check_gl_errors();
}

// pixels
void gl_pixel_storei( GLenum pname, GLint param)
{
	glPixelStorei( pname, param);
	check_gl_errors();
}

// transform
void gl_matrix_mode( GLenum mode)
{
	glMatrixMode( mode);
	check_gl_errors();
}

void gl_push_matrix()
{
	glPushMatrix();
	check_gl_errors();

	#ifndef NDEBUG
		++num_push_matrix;
	#endif
}

void gl_pop_matrix()
{
	glPopMatrix();
	check_gl_errors();

	#ifndef NDEBUG
		--num_push_matrix;
	#endif
}

void gl_load_identity()
{
	glLoadIdentity();
	check_gl_errors();
}

void gl_translatef( GLfloat x, GLfloat y, GLfloat z)
{
	glTranslatef( x, y, z);
	check_gl_errors();
}

void gl_scalef( GLfloat x, GLfloat y, GLfloat z)
{
	glScalef( x, y, z);
	check_gl_errors();
}

void gl_viewport( GLint x, GLint y, GLsizei width, GLsizei height)
{
	glViewport( x, y, width, height);
	check_gl_errors();
}

// draw
void gl_clear( GLbitfield mask)
{
	glClear( mask);
	check_gl_errors();
}

void gl_clear_color( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	glClearColor( red, green, blue, alpha);
	check_gl_errors();
}

void gl_begin( GLenum mode)
{
	RAMEN_ASSERT( inside_gl_begin_ == false);
	inside_gl_begin_ = true;
	glBegin( mode);
}

void gl_end()
{
	RAMEN_ASSERT( inside_gl_begin_ == true);
	inside_gl_begin_ = false;
	glEnd();
	check_gl_errors();
}

bool inside_begin_block() { return inside_gl_begin_;}

void gl_vertex2i( GLint x, GLint y)			{ glVertex2i( x, y);}
void gl_vertex2f( GLfloat x, GLfloat y)		{ glVertex2f( x, y);}

void gl_color3ub( GLubyte r, GLubyte g, GLubyte b)				{ glColor3ub( r, g, b);}
void gl_color3f( GLfloat r, GLfloat g, GLfloat b)				{ glColor3f( r, g, b);}
void gl_color4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a)	{ glColor4f( r, g, b, a);}

void gl_tex_coord2f( GLfloat s, GLfloat t) { glTexCoord2f( s, t);}

void gl_point_size( GLfloat size) { glPointSize( size);}
void gl_line_width( GLfloat width) { glLineWidth( width);}

// map & eval
void gl_map1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points)
{
	glMap1f( target, u1, u2, stride, order, points);
	check_gl_errors();
}

void gl_eval_coord1f( GLfloat u)
{
	glEvalCoord1f( u);
	check_gl_errors();
}

// more
void gl_flush()
{
	glFlush();
	check_gl_errors();
}

void gl_finish()
{
	glFinish();
	check_gl_errors();
}

// util
void gl_textured_quad( int x0, int y0, int x1, int y1, bool flip_tex_v)
{
	gl_begin( GL_QUADS);
		if( flip_tex_v)
		{
			gl_tex_coord2f( 0, 0);
			gl_vertex2i( x0, y0);
		
			gl_tex_coord2f( 1, 0);
			gl_vertex2i( x1, y0);
		
			gl_tex_coord2f( 1, 1);
			gl_vertex2i( x1, y1);
		
			gl_tex_coord2f( 0, 1);
			gl_vertex2i( x0, y1);
		}
		else
		{
			gl_tex_coord2f( 0, 1);
			gl_vertex2i( x0, y0);
		
			gl_tex_coord2f( 1, 1);
			gl_vertex2i( x1, y0);
		
			gl_tex_coord2f( 1, 0);
			gl_vertex2i( x1, y1);
		
			gl_tex_coord2f( 0, 0);
			gl_vertex2i( x0, y1);
		}
	gl_end();
}

} // ramen
