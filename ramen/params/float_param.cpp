// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/float_param.hpp>

#include<limits>

#include<boost/bind.hpp>

#include<QLabel>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/params/parameterised.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/viewer/viewer.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/param_spinbox.hpp>

namespace ramen
{

float_param_t::float_param_t( const std::string& name) : numeric_param_t( name) { private_init();}
float_param_t::float_param_t( const float_param_t& other) : numeric_param_t( other)
{
    input_ = 0;
}

void float_param_t::private_init()
{
    add_curve( name());
    set_default_value( 0);
}

poly_param_value_t float_param_t::value_at_frame( float frame) const
{
    float val = get_value<float>( *this);
    eval_curve( 0, frame, val);
    return poly_param_value_t( val);
}

void float_param_t::set_default_value( float x) { value().assign( x);}

void float_param_t::set_value( float x, change_reason reason)
{
    float frame = 1.0;

    if( composition_t * c = composition())
        frame = c->frame();

    set_value_at_frame( x, frame, reason);
}

void float_param_t::set_value_at_frame( float x, float frame, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    if( !is_static() && ( param_set()->autokey() || !curve().empty()))
    {
        curve().insert( frame, x);

        if( composition_t * c = composition())
            evaluate( c->frame());
    }
    else
        value().assign( x);

    emit_param_changed( reason);
}

void float_param_t::set_absolute_value( float x, change_reason reason)
{
    set_value( absolute_to_relative( round( x)), reason);
}

void float_param_t::set_absolute_value_at_frame( float x, float frame, change_reason reason)
{
    set_value_at_frame( absolute_to_relative( round( x)), frame, reason);
}

float float_param_t::derive( float time) const
{
    if( num_curves() && !curve().empty())
        return curve().derive( time);
    else
        return 0;
}

float float_param_t::integrate( float time1, float time2) const
{
    if( num_curves() && !curve().empty())
        return curve().integrate( time1, time2);
    else
    {
        float v = get_value<float>( *this);
        return (time2 - time1) * v;
    }
}

void float_param_t::do_format_changed( const Imath::Box2i& new_domain, float aspect, const Imath::V2f& proxy_scale)
{
    if( num_curves())
    {
        float scale, offset;
        get_scale_and_offset( scale, offset);
        curve().set_scale( scale);
        curve().set_offset( offset);
        app().ui()->update_anim_editors();
    }

    if( input_)
    {
        input_->blockSignals( true);
        input_->setRange( absolute_min(), absolute_max());
        input_->blockSignals( false);
        update_widgets();
    }
}

void float_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    hash_gen << get_value<float>( *this);
}

void float_param_t::do_read( const serialization::yaml_node_t& node)
{
    serialization::optional_yaml_node_t n = node.get_optional_node( "curve");

    if( n)
    {
        curve().read( n.get());
        return;
    }

    float val = 0;
    node.get_value<float>( "value", val);
    value().assign( val);
}

void float_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    {
        if( !curve().empty())
        {
            out << YAML::Key << "curve" << YAML::Value;
            curve().write( out);
        }
        else
            out << YAML::Key << "value" << YAML::Value << get_value<float>( *this);
    }
}

void float_param_t::do_update_widgets()
{
    if( input_)
    {
        input_->blockSignals( true);
        input_->setValue( relative_to_absolute( get_value<float>( *this)));
        input_->blockSignals( false);
    }
}

void float_param_t::do_enable_widgets( bool e)
{
    if( input_)
        input_->setEnabled( e);
}

QWidget *float_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    input_ = new ui::param_spinbox_t( *this, 0, top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());

    float low = absolute_min();
    float high = absolute_max();

    input_->setRange( low, high);

    input_->setValue( relative_to_absolute( get_value<float>( *this)));

    input_->setSingleStep( step());
    input_->move( app().ui()->inspector().left_margin(), 0);
    input_->resize( s.width(), s.height());
    input_->setEnabled( enabled());

    if( round_to_int())
        input_->setDecimals( 0);

    connect( input_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void float_param_t::value_changed( double value)
{
    param_set()->begin_edit();
    set_value( absolute_to_relative( round( value)));
    param_set()->end_edit();
}

void float_param_t::spinbox_pressed()
{
    if( track_mouse())
        app().ui()->begin_interaction();

    param_set()->begin_edit();
}

void float_param_t::spinbox_dragged( double value)
{
    set_value( absolute_to_relative( round( value)));

    if( track_mouse())
        param_set()->notify_parent();
    else
        parameterised()->update_overlay();

    app().ui()->update_anim_editors();
}

void float_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());

    if( track_mouse())
        app().ui()->end_interaction();
}

} // namespace
