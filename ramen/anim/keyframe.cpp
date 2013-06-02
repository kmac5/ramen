// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/keyframe.hpp>

#include<algorithm>

#include<ramen/assert.hpp>

namespace ramen
{
namespace anim
{

keyframe_t::keyframe_t() : time_( 0), selected_( false)
{
}

keyframe_t::keyframe_t( time_type time) : time_( time), selected_( false)
{
}
	
void keyframe_t::swap( keyframe_t& other)
{
	std::swap( time_, other.time_);
	std::swap( auto_v0_, other.auto_v0_);
	std::swap( auto_v1_, other.auto_v1_);
	std::swap( selected_, other.selected_);
}
	
void keyframe_t::swap_value( keyframe_t& other)
{
	std::swap( auto_v0_, other.auto_v0_);
	std::swap( auto_v1_, other.auto_v1_);
	std::swap( selected_, other.selected_);
}

keyframe_t::time_type keyframe_t::time_tolerance() { return 0.05;}

std::string keyframe_t::auto_tangent_to_string( auto_tangent_method t) const
{
	switch( t)
	{
	case tangent_step:
		return "step";
	case tangent_linear:
		return "linear";
	case tangent_smooth:
		return "smooth";
	case tangent_flat:
		return "flat";
	case tangent_fixed:
		return "fixed";
	}
}

keyframe_t::auto_tangent_method keyframe_t::string_to_auto_tangent( const std::string& s) const
{
	if( s == "step")
		return tangent_step;

	if( s == "linear")
		return tangent_linear;

	if( s == "smooth")
		return tangent_smooth;

	if( s == "flat")
		return tangent_flat;

	if( s == "fixed")
		return tangent_fixed;
	
	RAMEN_ASSERT( 0 && "Unknown auto-tangent method");
}
	
} // namespace
} // namespace
