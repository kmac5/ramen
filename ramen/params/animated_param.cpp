// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/animated_param.hpp>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/params/animated_param_command.hpp>

#include<ramen/anim/track.hpp>
#include<ramen/anim/clipboard.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{

animated_param_t::animated_param_t( const std::string& name) : param_t(name), step_( 1.0f)
{
}

animated_param_t::animated_param_t( const animated_param_t& other) : param_t( other), curves_( other.curves_)
{
    step_ = other.step_;
}

int animated_param_t::num_curves() const { return curves_.size();}

const std::string& animated_param_t::curve_name( int indx) const
{
	RAMEN_ASSERT( indx >= 0 && indx < num_curves());
	return boost::get<0>( curves_[indx]);
}

const anim::float_curve_t& animated_param_t::curve( int indx) const
{
	RAMEN_ASSERT( indx >= 0 && indx < num_curves());
	return boost::get<1>( curves_[indx]);
}

anim::float_curve_t& animated_param_t::curve( int indx)
{
	RAMEN_ASSERT( indx >= 0 && indx < num_curves());
	return boost::get<1>( curves_[indx]);
}

void animated_param_t::add_curve( const std::string& name)
{
	curves_.push_back( curve_entry_type( boost::flyweight<std::string>( name), anim::float_curve_t()));
}

void animated_param_t::eval_curve( int index, float frame, float& v) const
{
	if( num_curves() > 0)
	{
		RAMEN_ASSERT( index <= num_curves());
		
		if( !curve( index).empty())
			v = curve( index).evaluate( frame);
	}
}

float animated_param_t::get_min() const { return curve( 0).get_min();}

void animated_param_t::set_min( float lo)
{
    for( int i = 0; i < num_curves(); ++i)
		curve( i).set_min( lo);
}

float animated_param_t::get_max() const { return curve( 0).get_max();}

void animated_param_t::set_max( float hi)
{
    for( int i = 0; i < num_curves(); ++i)
		curve( i).set_max( hi);
}

void animated_param_t::set_range( float lo, float hi)
{
    for( int i = 0; i < num_curves(); ++i)
		curve( i).set_range( lo, hi);
}

void animated_param_t::set_default_anim_auto_tangents( anim::keyframe_t::auto_tangent_method m)
{
    for( int i = 0; i < num_curves(); ++i)
		curve( i).set_default_auto_tangents( m);
}

void animated_param_t::set_component_value( int index, float comp_value, change_reason reason)
{
    float frame = 1.0f;

    if( composition())
        frame = composition()->frame();

    set_component_value_at_frame( index, comp_value, frame, reason);
}

void animated_param_t::set_component_value_at_frame( int index, float comp_value, float frame, change_reason reason)
{
    RAMEN_ASSERT( index >= 0 && index < num_curves() && "animated_param_t: index out of bounds");

    if( can_undo())
        param_set()->add_command( this);

    if( poly_param_indexable_value_t *val =  adobe::poly_cast<poly_param_indexable_value_t*>( &value()))
    {
        RAMEN_ASSERT( val && "non indexable param value found");
        val->set_component( index, comp_value);
    }

    if( !is_static() && ( param_set()->autokey() || !curve( index).empty()))
        curve( index).insert( frame, comp_value);

    evaluate( frame);
	emit_param_changed( reason);
}

void animated_param_t::do_evaluate( float frame)
{
	value() = value_at_frame( frame);
}

float animated_param_t::clamp( float x) const
{
    if( x < get_min())
        return get_min();

    if( x > get_max())
        return get_max();

    return x;
}

void animated_param_t::anim_curve_changed( anim::any_curve_ptr_t& c) { do_anim_curve_changed( c);}

void animated_param_t::do_anim_curve_changed( anim::any_curve_ptr_t& c)
{
	RAMEN_ASSERT( composition());

	evaluate( composition()->frame());
    update_widgets();
	emit_param_changed( user_edited);
}

std::auto_ptr<undo::command_t> animated_param_t::do_create_command()
{
    return std::auto_ptr<undo::command_t>( new animated_param_command_t( *this->param_set(), id()));
}

// spinboxes
void animated_param_t::set_key( int curve_index)
{
    param_set()->begin_edit();

	if( can_undo())
		param_set()->add_command( this);

	if( curve( curve_index).empty())
	{
		poly_param_indexable_value_t *val =  adobe::poly_cast<poly_param_indexable_value_t*>( &value());
		
		if( val)
			curve( curve_index).insert( composition()->frame(), val->get_component( curve_index));
		else
		{
			RAMEN_ASSERT( curve_index == 0);
			
			float v = value().cast<float>();
			curve( 0).insert( composition()->frame(), v);
		}
	}
	else
		curve( curve_index).insert( composition()->frame());

    param_set()->end_edit( true);
    app().ui()->update_anim_editors();
}

void animated_param_t::delete_key( int curve_index, float time)
{
    param_set()->begin_edit();

	if( can_undo())
		param_set()->add_command( this);

	curve( curve_index).erase( time);
	curve( curve_index).recalc_tangents_and_coefficients();

    param_set()->end_edit( true);
    app().ui()->update_anim_editors();
}

void animated_param_t::delete_all_keys( int curve_index)
{
    param_set()->begin_edit();

	if( can_undo())
		param_set()->add_command( this);

	curve( curve_index).clear();

    param_set()->end_edit( true);
    app().ui()->update_anim_editors();
}

void animated_param_t::paste( int curve_index)
{
    param_set()->begin_edit();

	if( can_undo())
		param_set()->add_command( this);

	curve( curve_index).clear();
	anim::clipboard_t::instance().paste( curve( curve_index));
    param_set()->end_edit( true);
	
	evaluate( composition()->frame());
	update_widgets();
    app().ui()->update_anim_editors();
}

bool animated_param_t::all_curves_empty() const
{
	for( int i = 0; i < num_curves(); ++i)
    {
        if( !curve( i).empty())
            return false;
    }

    return true;
}

void animated_param_t::do_create_tracks( anim::track_t *parent)
{
	std::auto_ptr<anim::track_t> t;

	if( num_curves() == 1)
	{
		t.reset( new anim::track_t( curve_name(), &curve()));
	    t->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
	}
	else
	{
		t.reset( new anim::track_t( name()));

		for( int i = 0; i < num_curves(); ++i)
		{
			std::auto_ptr<anim::track_t> tx( new anim::track_t( curve_name( i), &curve( i)));
			tx->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
			t->add_child( tx);
		}
	}

	parent->add_child( t);
}

// serialization
void animated_param_t::read_curves( const serialization::yaml_node_t& node)
{
	serialization::optional_yaml_node_t curves( node.get_optional_node( "curves"));
	
	if( curves)
	{
		for( serialization::yaml_node_t::const_iterator it( curves.get().begin()); it != curves.get().end(); ++it)
		{
			std::string key;
			it.first() >> key;
			
			if( anim::float_curve_t *c = find_curve( key))
			{
				serialization::yaml_node_t crv_node( node, &( it.second()));
				c->read( crv_node);
			}
			else
				node.error_stream() << "Unknown curve " << key << " found in param.";
		}
	}
}

void animated_param_t::write_curves( serialization::yaml_oarchive_t& out) const
{
	if( !all_curves_empty())
	{
		out << YAML::Key << "curves" << YAML::Value;
		out.begin_map();

		for( int i = 0; i < num_curves(); ++i)
		{
			if( !curve( i).empty())
			{
				out << YAML::Key << curve_name( i) << YAML::Value;
				curve( i).write( out);
			}
		}

		out.end_map();
	}
}

anim::float_curve_t *animated_param_t::find_curve( const std::string& name)
{
	for( int i = 0; i < num_curves(); ++i)
	{
		if( curve_name( i) == name)
			return &curve( i);
	}
	
	return 0;
}

} // namespace
