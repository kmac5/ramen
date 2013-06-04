// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_GL_IMAGE_VIEW_SHADERS_HPP
#define	RAMEN_GL_IMAGE_VIEW_SHADERS_HPP

namespace ramen
{
namespace gl
{
namespace shaders
{

static const char *color_image_shader_source =
{
"uniform sampler2D tex;"
"uniform sampler3D lut3d;"
"uniform float exposure;"
"uniform vec4 channel_mask;"
"void main( void)"
"{"
    "vec4 col = texture2D( tex, gl_TexCoord[0].st);"
    "vec4 expv = vec4( exposure, exposure, exposure, 1);"
    "col *= expv;"
    "gl_FragColor = max( apply_lut( col, lut3d) * channel_mask, vec4( 0, 0, 0, 0));"
"}"
};

static const char *image_background_shader_source =
{
"uniform sampler3D lut3d;"
"void main( void)"
"{"
    "vec4 col = vec4( 0, 0, 0, 0);"
    "gl_FragColor = max( apply_lut( col, lut3d), vec4( 0, 0, 0, 0));"
"}"
};

static const char *alpha_shader_source =
{
"uniform sampler2D tex;"
"void main( void)"
"{"
    "vec4 col = texture2D( tex, gl_TexCoord[0].st);"
    "col = max( col, vec4( 0, 0, 0, 0));"
    "col = min( col, vec4( 1, 1, 1, 1));"
    "gl_FragColor = vec4( col.w, col.w, col.w, 1);"
"}"
};

} // namespace
} // namespace
} // namespace

#endif
