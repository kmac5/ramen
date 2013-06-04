// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OCIO_GL_LUT3D_HPP
#define	RAMEN_OCIO_GL_LUT3D_HPP

#include<ramen/GL/lut3d.hpp>

#include<string>

#include<OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

namespace ramen
{
namespace ocio
{

class gl_lut3d_t : public gl::lut3d_t
{
public:

    gl_lut3d_t( int lut_size = 32, GLenum texture_unit = GL_TEXTURE1);

    void recreate( OCIO::ConstConfigRcPtr config, OCIO::DisplayTransformRcPtr transform, const std::string& fun_name);

    void recreate( OCIO::ConstConfigRcPtr config, OCIO::DisplayTransformRcPtr transform,
				OCIO::ConstProcessorRcPtr processor, const std::string& fun_name);

    const std::string& lookup_function() const { return lut_fun_;}

	const Imath::Color3f& black() const;
	
private:

    std::string lut_fun_;
    std::string lut_cache_id_;
	Imath::Color3f black_;
};

} // namespace
} // namespace

#endif
