// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/camera/projection.hpp>

namespace ramen
{
namespace camera
{

void projection_t::reset()
{
	overscan_ = Imath::V2d( 0, 0);
	near_clipping_plane_ = 0.1;
	far_clipping_plane_  = 100000.0;
}

} // namespace
} // namespace
