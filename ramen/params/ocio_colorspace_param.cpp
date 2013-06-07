// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/ocio_colorspace_param.hpp>

#include<QComboBox>
#include<QLabel>

#include<ramen/params/param_set.hpp>
#include<ramen/nodes/node.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/ocio_colorspace_combo.hpp>

namespace ramen
{

ocio_colorspace_param_t::ocio_colorspace_param_t( const std::string& name) : static_param_t( name)
{
    set_default_value( default_colorspace());
}

ocio_colorspace_param_t::ocio_colorspace_param_t( const ocio_colorspace_param_t& other) : static_param_t( other), menu_( 0) {}

void ocio_colorspace_param_t::set_default_value( const std::string& cs) { value().assign( cs);}

void ocio_colorspace_param_t::set_value( const std::string& cs, change_reason reason)
{
    if( param_set() && can_undo())
        param_set()->add_command( this);

    value().assign( cs);
    emit_param_changed( reason);
}

QWidget *ocio_colorspace_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    menu_ = new ui::ocio_colorspace_combo_t( top);
    menu_->setFocusPolicy( Qt::NoFocus);

    QSize s = menu_->sizeHint();
    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());

    menu_->move( app().ui()->inspector().left_margin(), 0);
    menu_->resize( s.width(), s.height());

    std::string current_colorspace = get_value<std::string>( *this);
    menu_->set_colorspace_or_default( current_colorspace);
    menu_->setEnabled( enabled());
    connect( menu_, SIGNAL( colorspace_changed( const std::string&)), this, SLOT( colorspace_picked( const std::string&)));

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void ocio_colorspace_param_t::do_update_widgets()
{
    if( menu_)
    {
        menu_->blockSignals( true);

        std::string csname = get_value<std::string>( *this);
        menu_->set_colorspace( csname);
        menu_->blockSignals( false);
    }
}

void ocio_colorspace_param_t::do_enable_widgets( bool e)
{
    if( menu_)
        menu_->setEnabled( e);
}

void ocio_colorspace_param_t::colorspace_picked( const std::string& cs)
{
    param_set()->begin_edit();
    set_value( menu_->get_current_colorspace());
    param_set()->end_edit();
}

void ocio_colorspace_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    hash_gen << get_value<std::string>( *this);
}

void ocio_colorspace_param_t::do_read( const serialization::yaml_node_t& node)
{
    serialization::yaml_node_t n = node.get_node( "value");
    std::string val;
    n >> val;

    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
    int index = -1;

    int num_color_spaces = config->getNumColorSpaces();

    for(int i = 0; i < num_color_spaces; i++)
    {
        std::string csname = config->getColorSpaceNameByIndex( i);

        if( csname == val)
            index = i;
    }

    if( index != -1)
        value().assign( val);
    else
    {
        node.error_stream() << "Node " << parameterised()->name() << ": colorspace " << val << " not found in OCIO config.\n";
        node.error_stream() << "Replacing by default value.\n";
        value().assign( default_colorspace());
    }
}

void ocio_colorspace_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "value" << YAML::Value << get_value<std::string>( *this);
}

std::string ocio_colorspace_param_t::default_colorspace() const
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
    return config->getColorSpace( OCIO::ROLE_SCENE_LINEAR)->getName();
}

} // namespace
