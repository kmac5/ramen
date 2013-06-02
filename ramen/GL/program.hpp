// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_GL_PROGRAM_HPP
#define	RAMEN_GL_PROGRAM_HPP

#include<boost/noncopyable.hpp>

#include<ramen/GL/gl.hpp>

namespace ramen
{
namespace gl
{

class program_t : boost::noncopyable
{
public:

	program_t( GLenum shader_type = GL_FRAGMENT_SHADER);
	~program_t();

	void compile( const char *src0, const char *src1 = 0);

	void bind();
	void unbind();

    void uniform_1i( const char *param, int v);
    void uniform_1f( const char *param, float v);

    void uniform_4f( const char *param, float *v);

private:

	GLenum shader_type_;
    GLuint program_, shader_;
};

} // namespace
} // namespace

#endif
