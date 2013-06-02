// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ocio/gl_lut3d.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace ocio
{

gl_lut3d_t::gl_lut3d_t( int lut_size, GLenum texture_unit) : gl::lut3d_t( lut_size, texture_unit)
{
	black_ = Imath::Color3f( 0, 0, 0);
}

void gl_lut3d_t::recreate( OCIO::ConstConfigRcPtr config, OCIO::DisplayTransformRcPtr transform, const std::string& fun_name)
{
    RAMEN_ASSERT( transform);
    OCIO::ConstProcessorRcPtr processor = config->getProcessor( transform);
	recreate( config, transform, processor, fun_name);
}

void gl_lut3d_t::recreate( OCIO::ConstConfigRcPtr config, OCIO::DisplayTransformRcPtr transform,
							OCIO::ConstProcessorRcPtr processor, const std::string& fun_name)
{
    OCIO::GpuShaderDesc desc;
    desc.setLanguage( OCIO::GPU_LANGUAGE_GLSL_1_0);
    desc.setFunctionName( fun_name.c_str());
    desc.setLut3DEdgeLen( lut_size());

    std::string cache_id = processor->getGpuLut3DCacheID( desc);

    if( cache_id == lut_cache_id_)
        return;

    lut_cache_id_ = cache_id;
    lut_fun_ = processor->getGpuShaderText( desc);
    processor->getGpuLut3D( reinterpret_cast<float*>( data()), desc);
	black_ = data()[0];
	update_gl_texture();
}

const Imath::Color3f& gl_lut3d_t::black() const { return black_;}

} // namespace
} // namespace
