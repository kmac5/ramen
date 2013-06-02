// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/float3_param.hpp>

#include<limits>

#include<boost/bind.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<QLabel>

#include<ramen/params/parameterised.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/viewer/viewer.hpp>

#include<ramen/ui/widgets/param_spinbox.hpp>

#include<ramen/python/util.hpp>

namespace ramen
{

float3_param_t::float3_param_t( const std::string& name) : proportional_param_t( name) { private_init();}

float3_param_t::float3_param_t( const float3_param_t& other) : proportional_param_t( other)
{
    input0_ = input1_ = input2_ = 0;
}

void float3_param_t::private_init()
{
    add_expression( "X");
    add_expression( "Y");
    add_expression( "Z");

    add_curve( "X");
    add_curve( "Y");
    add_curve( "Z");
    set_default_value( Imath::V3f( 0, 0, 0));
}

poly_param_value_t float3_param_t::value_at_frame( float frame) const
{
    Imath::V3f v( get_value<Imath::V3f>( *this));

    if( !eval_expression( 0, frame, v.x, input0_))
        eval_curve( 0, frame, v.x);

    if( !eval_expression( 1, frame, v.y, input1_))
        eval_curve( 1, frame, v.y);

    if( !eval_expression( 2, frame, v.z, input2_))
        eval_curve( 2, frame, v.z);

    poly_param_indexable_value_t val( v);
    return adobe::poly_cast<poly_param_value_t&>( val);
}

void float3_param_t::set_default_value( const Imath::V3f& x)
{
    poly_param_indexable_value_t v( x);
    value() = adobe::poly_cast<poly_param_value_t&>( v);
}

void float3_param_t::set_value( const Imath::V3f& x, change_reason reason)
{
    float frame = 1.0f;

    if( composition_t * c = composition())
        frame = c->frame();

    set_value_at_frame( x, frame, reason);
}

void float3_param_t::set_value_at_frame( const Imath::V3f& x, float frame, change_reason reason)
{
    //RAMEN_ASSERT( expression( 0).empty());
    //RAMEN_ASSERT( expression( 1).empty());
    //RAMEN_ASSERT( expression( 2).empty());

    if( can_undo())
        param_set()->add_command( this);

    poly_param_indexable_value_t v( x);
    value() = adobe::poly_cast<poly_param_value_t&>( v);

    bool autokey = param_set()->autokey();

    if( !is_static() && ( autokey || !curve( 0).empty()))
        curve( 0).insert( frame, x.x);

    if( !is_static() && ( autokey || !curve( 1).empty()))
        curve( 1).insert( frame, x.y);

    if( !is_static() && ( autokey || !curve( 2).empty()))
        curve( 2).insert( frame, x.z);

    if( composition_t * c = composition())
        evaluate( c->frame());

    emit_param_changed( reason);
}

Imath::V3f float3_param_t::derive( float time) const
{
    Imath::V3f result( 0.0f, 0.0f, 0.0f);

    if( !curve( 0).empty())
        result.x = curve( 0).derive( time);

    if( !curve( 1).empty())
        result.y = curve( 1).derive( time);

    if( !curve( 2).empty())
        result.z = curve( 2).derive( time);

    return result;
}

Imath::V3f float3_param_t::integrate( float time1, float time2) const
{
    Imath::V3f result = get_value<Imath::V3f>( *this);
    result.x = (time2 - time1) * result.x;
    result.y = (time2 - time1) * result.y;
    result.z = (time2 - time1) * result.z;

    if( !curve( 0).empty())
        result.x = curve( 0).integrate( time1, time2);

    if( !curve( 1).empty())
        result.y = curve( 1).integrate( time1, time2);

    if( !curve( 2).empty())
        result.z = curve( 2).integrate( time1, time2);

    return result;
}

void float3_param_t::set_component_value_from_slot()
{
    Imath::V3f v( round (input0_->value()), round( input1_->value()), round( input2_->value()));
    int index;
    float comp_value;

    if( QObject::sender() == input0_)
    {
        index = 0;
        comp_value = v.x;
    }
    else
    {
        if( QObject::sender() == input1_)
        {
            index = 1;
            comp_value = v.y;
        }
        else
        {
            index = 2;
            comp_value = v.z;
        }
    }

    set_component_value( index, comp_value);
}

void float3_param_t::calc_proportional_factors()
{
    proportional_factor = Imath::V3f( 1, 1, 1);
    Imath::V3f v = get_value<Imath::V3f>( *this);

    if( sender() == input0_)
    {
        if( v.x)
        {
            proportional_factor.y = ( v.y / v.x) != 0 ? v.y / v.x : 1;
            proportional_factor.z = ( v.z / v.x) != 0 ? v.z / v.x : 1;
        }
    }
    else
    {
        if( sender() == input1_)
        {
            if( v.y)
            {
                proportional_factor.x = ( v.x / v.y) != 0 ? v.x / v.y : 1;
                proportional_factor.z = ( v.z / v.y) != 0 ? v.z / v.y : 1;
            }
        }
        else
        {
            if( v.z)
            {
                proportional_factor.x = ( v.x / v.z) != 0 ? v.x / v.z : 1;
                proportional_factor.y = ( v.y / v.z) != 0 ? v.y / v.z : 1;
            }
        }
    }
}

void float3_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    Imath::V3f v( get_value<Imath::V3f>( *this));
    hash_gen << v.x << "," << v.y << "," << v.z;
}

boost::python::object float3_param_t::to_python( const poly_param_value_t& v) const
{
    return python::vec_to_list( v.cast<Imath::V3f>());
}

poly_param_value_t float3_param_t::from_python( const boost::python::object& obj) const
{
    boost::python::list t = boost::python::extract<boost::python::list>( obj);
    Imath::V3f val = python::list_to_vec3<float>( t);
    poly_param_indexable_value_t v( val);
    return adobe::poly_cast<poly_param_value_t&>( v);
}

void float3_param_t::do_read( const serialization::yaml_node_t& node)
{
    read_expressions( node);
    read_curves( node);

    Imath::V3f val;
    if( node.get_optional_value( "value", val))
    {
        poly_param_indexable_value_t v( val);
        value().assign( adobe::poly_cast<poly_param_value_t&>( v));
    }
}

void float3_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    write_expressions( out);
    write_curves( out);

    bool one   = curve( 0).empty(); // && expression( 0).empty();
    bool two   = curve( 1).empty(); // && expression( 1).empty();
    bool three = curve( 2).empty(); // && expression( 2).empty();

    if( one || two || three)
        out << YAML::Key << "value" << YAML::Value << get_value<Imath::V3f>( *this);
}

QWidget *float3_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    input0_ = new ui::param_spinbox_t( *this, 0, top);
    input1_ = new ui::param_spinbox_t( *this, 1, top);
    input2_ = new ui::param_spinbox_t( *this, 2, top);

    QSize s = input0_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());
    Imath::V3f triple = get_value<Imath::V3f>( *this);

    // make inputs bigger
    s.setWidth( s.width() + ( s.width() / 4));

    int xpos = app().ui()->inspector().left_margin();

    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setRange( get_min(), get_max());
    input0_->setSingleStep( step());
    input0_->setValue( triple.x);
    input0_->setEnabled( enabled());
    if( round_to_int()) input0_->setDecimals( 0);
    connect( input0_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input0_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input0_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input0_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    connect( input0_, SIGNAL( expressionSet()), this, SLOT( expression_set()));
    xpos += ( s.width() + 5);

    input1_->move( xpos, 0);
    input1_->resize( s.width(), s.height());
    input1_->setRange( get_min(), get_max());
    input1_->setSingleStep( step());
    input1_->setValue( triple.y);
    input1_->setEnabled( enabled());
    if( round_to_int())	input1_->setDecimals( 0);
    connect( input1_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    connect( input1_, SIGNAL( expressionSet()), this, SLOT( expression_set()));
    xpos += ( s.width() + 5);

    input2_->move( xpos, 0);
    input2_->resize( s.width(), s.height());
    input2_->setRange( get_min(), get_max());
    input2_->setSingleStep( step());
    input2_->setValue( triple.z);
    input2_->setEnabled( enabled());
    if( round_to_int()) input2_->setDecimals( 0);
    connect( input2_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input2_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input2_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input2_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    connect( input2_, SIGNAL( expressionSet()), this, SLOT( expression_set()));
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

void float3_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);
        input2_->blockSignals( true);

        Imath::V3f triple = get_value<Imath::V3f>( *this);
        input0_->setValue( triple.x);
        input1_->setValue( triple.y);
        input2_->setValue( triple.z);

        input0_->blockSignals( false);
        input1_->blockSignals( false);
        input2_->blockSignals( false);
    }
}

void float3_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);
        input2_->setEnabled( e);

        if( prop_button_)
            prop_button_->setEnabled( e);
    }
}

void float3_param_t::value_changed( double value)
{
    param_set()->begin_edit();

    if( proportional_checked())
    {
        calc_proportional_factors();
        Imath::V3f v = get_value<Imath::V3f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = clamp( v.y + ( inc * proportional_factor.y));
            v.z = clamp( v.z + ( inc * proportional_factor.z));
        }
        else
        {
            if( sender() == input1_)
            {
                float inc = value - v.y;
                v.x = clamp( v.x + ( inc * proportional_factor.x));
                v.y = value;
                v.z = clamp( v.z + ( inc * proportional_factor.z));
            }
            else
            {
                float inc = value - v.z;
                v.x = clamp( v.x + ( inc * proportional_factor.x));
                v.y = clamp( v.y + ( inc * proportional_factor.y));
                v.z = value;
            }
        }

        set_value( round( v));
        update_widgets();
    }
    else
        set_component_value_from_slot();

    param_set()->end_edit();
}

void float3_param_t::spinbox_pressed()
{
    if( track_mouse())
        app().ui()->begin_interaction();

    param_set()->begin_edit();

    if( proportional_checked())
        calc_proportional_factors();
}

void float3_param_t::spinbox_dragged( double value)
{
    if( proportional_checked())
    {
        Imath::V3f v = get_value<Imath::V3f>( *this);

        if( sender() == input0_)
        {
            float inc = value - v.x;
            v.x = value;
            v.y = adobe::clamp( v.y + ( inc * proportional_factor.y), get_min(), get_max());
            v.z = adobe::clamp( v.z + ( inc * proportional_factor.z), get_min(), get_max());
        }
        else
        {
            if( sender() == input1_)
            {
                float inc = value - v.y;
                v.x = adobe::clamp( v.x + ( inc * proportional_factor.x), get_min(), get_max());
                v.y = value;
                v.z = adobe::clamp( v.z + ( inc * proportional_factor.z), get_min(), get_max());
            }
            else
            {
                float inc = value - v.z;
                v.x = adobe::clamp( v.x + ( inc * proportional_factor.x), get_min(), get_max());
                v.y = adobe::clamp( v.y + ( inc * proportional_factor.y), get_min(), get_max());
                v.z = value;
            }
        }

        set_value( round ( v));
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

void float3_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());

    if( track_mouse())
        app().ui()->end_interaction();
}

void float3_param_t::expression_set()
{
}

} // namespace
