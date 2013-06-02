// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/aspect_ratio_param.hpp>

#include<QComboBox>
#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/double_spinbox.hpp>

namespace ramen
{

aspect_ratio_param_t::presets_type aspect_ratio_param_t::presets_;

aspect_ratio_param_t::aspect_ratio_param_t( const std::string& name) : static_param_t( name)
{
    set_default_value( 1.0f);
}

aspect_ratio_param_t::aspect_ratio_param_t( const aspect_ratio_param_t& other) : static_param_t( other)
{
    input_ = 0;
    menu_ = 0;
}

void aspect_ratio_param_t::set_default_value( float x) { value().assign( x);}

void aspect_ratio_param_t::set_value( float x, change_reason reason)
{
    if( param_set() && can_undo())
        param_set()->add_command( this);

    value().assign( x);
    emit_param_changed( reason);
}

void aspect_ratio_param_t::do_add_to_hash( hash::generator_t& hash_gen) const { hash_gen << get_value<float>( *this);}

boost::python::object aspect_ratio_param_t::to_python( const poly_param_value_t& v) const
{
    return boost::python::object( get_value<float>( *this));
}

poly_param_value_t aspect_ratio_param_t::from_python( const boost::python::object& obj) const
{
    float val = boost::python::extract<float>( obj);
    return poly_param_value_t( val);
}

void aspect_ratio_param_t::do_read( const serialization::yaml_node_t& node)
{
    float val = 1.0f;
    node.get_value<float>( "value", val);
    set_value( val, silent_edit);
}

void aspect_ratio_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "value"
        << YAML::Value << get_value<float>( *this);
}

void aspect_ratio_param_t::do_update_widgets()
{
    if( menu_)
    {
        menu_->blockSignals( true);
        input_->blockSignals( true);

        float val = get_value<float>( *this);
        input_->setValue( val);
        menu_->setCurrentIndex( index_for_value( val));

        menu_->blockSignals( false);
        input_->blockSignals( false);
    }
}

void aspect_ratio_param_t::do_enable_widgets( bool e)
{
    if( menu_)
    {
        menu_->setEnabled( e);
        input_->setEnabled( e);
    }
}

void aspect_ratio_param_t::init_presets()
{
    static bool inited = false;

    if( !inited)
    {
        presets_.push_back( std::make_pair( "Square", 1.0f));
        presets_.push_back( std::make_pair( "PAL 1.067", 1.067f));
        presets_.push_back( std::make_pair( "NTSC 0.91", 0.91f));
        presets_.push_back( std::make_pair( "HD 1440 1.33", 1.33f));
        presets_.push_back( std::make_pair( "PAL 16:9 1.46", 1.46f));
        presets_.push_back( std::make_pair( "NTSC 16:9 1.21", 1.21f));
        presets_.push_back( std::make_pair( "Cinemascope 2", 2.0f));
        inited = true;
    }
}

const aspect_ratio_param_t::presets_type& aspect_ratio_param_t::presets()
{
    init_presets();
    return presets_;
}

void aspect_ratio_param_t::add_preset( const std::pair<std::string, float>& p)
{
    for( int i = 0; i < presets_.size(); ++i)
    {
        if( p == presets_[i])
            return;
    }

    presets_.push_back( p);
}

int aspect_ratio_param_t::index_for_value( float x) const
{
    for( int i = 0; i < presets().size(); ++i)
    {
        if( presets()[i].second == x)
            return i;
    }

    return presets().size();
}

QWidget *aspect_ratio_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    menu_ = new QComboBox( top);
    input_ = new ui::double_spinbox_t( top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());

    input_->move( app().ui()->inspector().left_margin(), 0);
    float val = get_value<float>( *this);
    input_->setValue( val);
    input_->setEnabled( enabled());
    input_->setDecimals( 3);
    input_->setMinimum( 0.1);
    input_->setTrackMouse( false);
    connect( input_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));

    menu_->setFocusPolicy( Qt::NoFocus);

    for( int i = 0; i < presets().size(); ++i)
        menu_->addItem( presets()[i].first.c_str());

    menu_->addItem( "Custom");

    menu_->move( app().ui()->inspector().left_margin() + s.width() + 5, 0);
    menu_->setEnabled( enabled());
    menu_->setCurrentIndex( index_for_value( val));
    connect( menu_, SIGNAL( currentIndexChanged( int)), this, SLOT( item_picked( int)));

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void aspect_ratio_param_t::item_picked( int index)
{
    if( index == presets().size())
        return;

    param_set()->begin_edit();
    float val = presets()[index].second;
    set_value( val);

    input_->blockSignals( true);
    input_->setValue( val);
    input_->blockSignals( false);

    param_set()->end_edit();
}

void aspect_ratio_param_t::value_changed( double value)
{
    param_set()->begin_edit();
    set_value( value);

    menu_->blockSignals( true);
    menu_->setCurrentIndex( index_for_value( value));
    menu_->blockSignals( false);

    param_set()->end_edit();
}

} // namespace
