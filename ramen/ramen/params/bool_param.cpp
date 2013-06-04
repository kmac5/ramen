// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/bool_param.hpp>

#include<ramen/app/application.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>

namespace ramen
{

bool_param_t::bool_param_t( const std::string& name) : static_param_t( name)
{
    set_default_value( false);
}

bool_param_t::bool_param_t( const bool_param_t& other) : static_param_t( other)
{
    button_ = 0;
}

void bool_param_t::set_default_value( bool x) { value().assign( x);}

void bool_param_t::set_value( bool x, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    value().assign( x);
    emit_param_changed( reason);
}

param_t *bool_param_t::do_clone() const { return new bool_param_t( *this);}

void bool_param_t::do_add_to_hash( hash::generator_t& hash_gen) const { hash_gen << get_value<bool>( *this);}

boost::python::object bool_param_t::to_python( const poly_param_value_t& v) const
{
    return boost::python::object( v.cast<bool>());
}

poly_param_value_t bool_param_t::from_python( const boost::python::object& obj) const
{
    bool b = boost::python::extract<bool>( obj);
    return poly_param_value_t( b);
}

void bool_param_t::do_read( const serialization::yaml_node_t& node)
{
    bool val;
    node.get_value<bool>( "value", val);
    set_value( val, silent_edit);
}

void bool_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "value"
        << YAML::Value << get_value<bool>( *this);
}

void bool_param_t::do_update_widgets()
{
    if( button_)
    {
        button_->blockSignals( true);
        button_->setChecked( get_value<bool>( *this));
        button_->blockSignals( false);
    }
}

void bool_param_t::do_enable_widgets( bool e)
{
    if( button_)
        button_->setEnabled( e);
}

QWidget *bool_param_t::do_create_widgets()
{
    QWidget *w = new QWidget();

    button_ = new QCheckBox( w);
    button_->setFocusPolicy( Qt::NoFocus);
    button_->setText( name().c_str());
    QSize s = button_->sizeHint();

    w->setMinimumSize( app().ui()->inspector().width(), s.height());
    w->setMaximumSize( app().ui()->inspector().width(), s.height());

    button_->move( app().ui()->inspector().left_margin(), 0);
    button_->resize( s.width(), s.height());
    button_->setChecked( get_value<bool>( *this));
    button_->setEnabled( enabled());
    button_->setToolTip( id().c_str());

    connect( button_, SIGNAL( stateChanged( int)), this, SLOT( button_checked( int)));
    return w;
}

void bool_param_t::button_checked( int state)
{
    param_set()->begin_edit();
    set_value( state);
    param_set()->end_edit();
}

} // namespace
