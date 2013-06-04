// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_KEYFRAME_HPP
#define RAMEN_ANIM_KEYFRAME_HPP

#include<ramen/config.hpp>

#include<string>

namespace ramen
{
namespace anim
{

class keyframe_t
{
public:

    typedef float time_type;

    enum auto_tangent_method
    {
        tangent_step = 0,
        tangent_linear,
        tangent_smooth,
        tangent_flat,
        tangent_fixed
    };

    keyframe_t();
    keyframe_t( time_type time);
	
	void swap( keyframe_t& other);
	void swap_value( keyframe_t& other);
	
    time_type time() const	{ return time_;}
    void set_time( time_type t) { time_ = t;}

    bool selected() const           { return selected_;}
    void select( bool b) const      { selected_ = b;}
    void toggle_selection() const   { selected_ = !selected_;}

    auto_tangent_method v0_auto_tangent() const			{ return auto_v0_;}
    void set_v0_auto_tangent( auto_tangent_method m)	{ auto_v0_ = m;}

    auto_tangent_method v1_auto_tangent() const			{ return auto_v1_;}
    void set_v1_auto_tangent( auto_tangent_method m)	{ auto_v1_ = m;}
	
    bool operator<( const keyframe_t& other) const { return time_ < other.time_;}

	static time_type time_tolerance();
	
protected:

    std::string auto_tangent_to_string( auto_tangent_method t) const;
	auto_tangent_method string_to_auto_tangent( const std::string& s) const;
	
    time_type time_;
	
    auto_tangent_method auto_v0_, auto_v1_;
	
    mutable bool selected_;
};

} // namespace
} // namespace

#endif
