// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/float2_param.hpp>

#include<limits>

#include<boost/bind.hpp>

#include<QLabel>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/algorithm/clamp.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/widgets/param_spinbox.hpp>
#include<ramen/ui/inspector/inspector.hpp>

namespace ramen
{

float2_param_t::float2_param_t( const std::string& name) : proportional_param_t( name) { private_init();}

float2_param_t::float2_param_t( const float2_param_t& other) : proportional_param_t( other)
{
    input0_ = input1_ = 0;
}

void float2_param_t::private_init()
{
    add_curve( "X");
    add_curve( "Y");
    set_default_value( Imath::V2f( 0, 0));
}

void float2_param_t::set_default_value( const Imath::V2f& x)
{
    poly_param_indexable_value_t v( x);
    value() = core::poly_cast<poly_param_value_t&>( v);
}

poly_param_value_t float2_param_t::value_at_frame( float frame) const
{
    Imath::V2f v( get_value<Imath::V2f>( *this));
    eval_curve( 0, frame, v.x);
    eval_curve( 1, frame, v.y);
    poly_param_indexable_value_t val( v);
    return core::poly_cast<poly_param_value_t&>( val);
}

void float2_param_t::set_value( const Imath::V2f& x, change_reason reason)
{
    float frame = 1.0f;

    if( composition_t * c = composition())
        frame = c->frame();

    set_value_at_frame( x, frame, reason);
}

void float2_param_t::set_value_at_frame( const Imath::V2f& x, float frame, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    poly_param_indexable_value_t v( x);
    value() = core::poly_cast<poly_param_value_t&>( v);

    bool autokey = param_set()->autokey();

    if( !is_static() && ( autokey || !curve( 0).empty()))
        curve( 0).insert( frame, x.x);

    if( !is_static() && ( autokey || !curve( 1).empty()))
        curve( 1).insert( frame, x.y);

    if( composition_t * c = composition())
        evaluate( c->frame());

    emit_param_changed( reason);
}

void float2_param_t::set_absolute_value( const Imath::V2f& x, change_reason reason)
{
    set_value( absolute_to_relative( round( x)), reason);
}

void float2_param_t::set_absolute_value_at_frame( const Imath::V2f& x, float frame, change_reason reason)
{
    set_value_at_frame( absolute_to_relative( round( x)), frame, reason);
}

Imath::V2f float2_param_t::derive( float time) const
{
    Imath::V2f result( 0.0f, 0.0f);

    if( !curve( 0).empty())
        result.x = curve( 0).derive( time);

    if( !curve( 1).empty())
        result.y = curve( 1).derive( time);

    return result;
}

Imath::V2f float2_param_t::integrate( float time1, float time2) const
{
    Imath::V2f result = get_value<Imath::V2f>( *this);
    result.x  = (time2 - time1) * result.x;
    result.y  = (time2 - time1) * result.y;

    if( !curve( 0).empty())
        result.x = curve( 0).integrate( time1, time2);

    if( !curve( 1).empty())
        result.y = curve( 1).integrate( time1, time2);

    return result;
}

void float2_param_t::do_format_changed( const Imath::Box2i& new_domain, float aspect, const Imath::V2f& proxy_scale)
{
    Imath::V2f scale, offset;
    get_scale_and_offset( scale, offset);
    curve( 0).set_scale( scale.x);
    curve( 0).set_offset( offset.x);
    curve( 1).set_scale( scale.y);
    curve( 1).set_offset( offset.y);
    app().ui()->update_anim_editors();

    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);

        input0_->setRange( absolute_min(), absolute_max());
        input1_->setRange( absolute_min(), absolute_max());

        input0_->blockSignals( false);
        input1_->blockSignals( false);

        update_widgets();
    }
}

void float2_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    Imath::V2f v( get_value<Imath::V2f>( *this));
    hash_gen << v.x << "," << v.y;
}

void float2_param_t::do_read( const serialization::yaml_node_t& node)
{
    read_curves( node);

    Imath::V2f val;
    if( node.get_optional_value( "value", val))
    {
        poly_param_indexable_value_t v( val);
        value().assign( core::poly_cast<poly_param_value_t&>( v));
    }
}

void float2_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    write_curves( out);

    bool one = curve( 0).empty(); // && expression( 0).empty();
    bool two = curve( 1).empty(); // && expression( 1).empty();

    if( one || two)
        out << YAML::Key << "value" << YAML::Value << get_value<Imath::V2f>( *this);
}

void float2_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);

        Imath::V2f v = get_absolute_value<Imath::V2f>( *this);
        input0_->setValue( v.x);
        input1_->setValue( v.y);

        input0_->blockSignals( false);
        input1_->blockSignals( false);
    }
}

void float2_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);

        if( prop_button_)
            prop_button_->setEnabled( e);
    }
}

QWidget *float2_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    input0_ = new ui::param_spinbox_t( *this, 0, top);
    input1_ = new ui::param_spinbox_t( *this, 1, top);

    QSize s = input0_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());

    int xpos = app().ui()->inspector().left_margin();

    Imath::V2f v = get_absolute_value<Imath::V2f>( *this);
    float low = absolute_min();
    float high = absolute_max();

    // make inputs a little bigger
    s.setWidth( s.width() + ( s.width() / 4));

    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setRange( low, high);
    input0_->setSingleStep( step());
    input0_->setValue( v.x);
    input0_->setEnabled( enabled());
    if( round_to_int()) input0_->setDecimals( 0);
    connect( input0_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input0_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input0_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input0_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 5);

    input1_->move( xpos, 0);
    input1_->resize( s.width(), s.height());
    input1_->setRange( low, high);
    input1_->setSingleStep( step());
    input1_->setValue( v.y);
    input1_->setEnabled( enabled());
    if( round_to_int()) input1_->setDecimals( 0);
    connect( input1_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += ( s.width() + 2);

    if( proportional())
    {
        create_proportional_button( top, s.height());

        if( prop_button_)
            prop_button_->move( xpos, 0);
    }

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void float2_param_t::calc_proportional_factors()
{

    proportional_factor = Imath::V3f( 1, 1, 1);
    Imath::V2f v = get_absolute_value<Imath::V2f>( *this);

    if( sender() == input0_)
    {
        if( v.x)
            proportional_factor.y = ( v.y / v.x) != 0 ? v.y / v.x : 1;
    }
    else
    {
        if( v.y)
            proportional_factor.x = ( v.x / v.y) != 0 ? v.x / v.y : 1;
    }
}

void float2_param_t::set_component_value_from_slot()
{
    Imath::V2f v( absolute_to_relative( Imath::V2f( round (input0_->value()), round( input1_->value()))));
    int index;
    float comp_value;

    if( QObject::sender() == input0_)
    {
        index = 0;
        comp_value = v.x;
    }
    else
    {
        index = 1;
        comp_value = v.y;
    }

    set_component_value( index, comp_value);
}

void float2_param_t::value_changed( double value)
{
    param_set()->begin_edit();

    if( proportional_checked())
    {
        calc_proportional_factors();
        Imath::V2f v = get_absolute_value<Imath::V2f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = clamp( v.y + ( inc * proportional_factor.y));
        }
        else
        {
            float inc = value - v.y;
            v.y = value;
            v.x = clamp( v.x + ( inc * proportional_factor.x));
        }

        set_value( absolute_to_relative( round( v)));
        update_widgets();
    }
    else
        set_component_value_from_slot();

    param_set()->end_edit();
}

void float2_param_t::spinbox_pressed()
{
    if( track_mouse())
        app().ui()->begin_interaction();

    param_set()->begin_edit();

    if( proportional_checked())
        calc_proportional_factors();
}

void float2_param_t::spinbox_dragged( double value)
{
    if( proportional_checked())
    {
        Imath::V2f v = get_absolute_value<Imath::V2f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = ramen::clamp( v.y + ( inc * proportional_factor.y), absolute_min(), absolute_max());
        }
        else
        {
            float inc = value - v.y;
            v.y = value;
            v.x = ramen::clamp( v.x + ( inc * proportional_factor.x), absolute_min(), absolute_max());
        }

        set_value( absolute_to_relative( round ( v)));
        update_widgets();
    }
    else
        set_component_value_from_slot();

    if( track_mouse())
        param_set()->notify_parent();
    else
        parameterised()->update_overlay();

    app().ui()->update_anim_editors();
}

void float2_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());

    if( track_mouse())
        app().ui()->end_interaction();
}

} // namespace
