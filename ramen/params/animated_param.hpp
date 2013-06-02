// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIMATED_PARAM_HPP
#define	RAMEN_ANIMATED_PARAM_HPP

#include<ramen/params/animated_param_fwd.hpp>

#include<ramen/params/param.hpp>

#include<boost/tuple/tuple.hpp>

#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/any_curve.hpp>

#include<ramen/serialization/archive_fwd.hpp>

namespace ramen
{

class RAMEN_API animated_param_t : public param_t
{
    Q_OBJECT

public:

	typedef anim::float_key_t::time_type time_type;

    explicit animated_param_t( const std::string& name);

	int num_curves() const;
	
	const std::string& curve_name( int indx = 0) const;
	const anim::float_curve_t& curve( int indx = 0) const;
	anim::float_curve_t& curve( int indx = 0);

    float get_min() const;
    void set_min( float lo);

    float get_max() const;
    void set_max( float hi);

    void set_range( float lo, float hi);

    float step() const	    { return step_;}
    void set_step( float s) { step_ = s;}

	void set_default_anim_auto_tangents( anim::keyframe_t::auto_tangent_method m);
	
    void anim_curve_changed( anim::any_curve_ptr_t& c);

	// spinbox
	void set_key( int curve_index);
	void delete_key( int curve_index, float time);
	void delete_all_keys( int curve_index);
	void paste( int curve_index);
	
protected:

    animated_param_t( const animated_param_t& other);
    void operator=( const animated_param_t& other);

    bool all_curves_empty() const;
    void add_curve( const std::string& name);
	void eval_curve( int index, float frame, float& v) const;
	
    void set_component_value( int index, float comp_value, change_reason reason = user_edited);
    void set_component_value_at_frame( int index, float comp_value, float frame, change_reason reason = user_edited);

    float clamp( float x) const;

    virtual void do_anim_curve_changed( anim::any_curve_ptr_t& c);

    // serialization
	void read_curves( const serialization::yaml_node_t& node);
	void write_curves( serialization::yaml_oarchive_t& out) const;

private:

    virtual void do_evaluate( float frame);

    virtual void do_create_tracks( anim::track_t *parent);

    virtual std::auto_ptr<undo::command_t> do_create_command();

	anim::float_curve_t *find_curve( const std::string& name);
	
	typedef boost::tuple<boost::flyweight<std::string>, anim::float_curve_t> curve_entry_type;
    std::vector<curve_entry_type> curves_;

    float step_;
};

} // namespace

#endif
