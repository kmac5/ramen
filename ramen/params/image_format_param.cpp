// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/image_format_param.hpp>

#include<QLabel>
#include<QComboBox>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/spinbox.hpp>
#include<ramen/ui/widgets/double_spinbox.hpp>

namespace ramen
{

image_format_param_t::image_format_param_t( const std::string& name) : static_param_t( name) {}

image_format_param_t::image_format_param_t( const image_format_param_t& other) : static_param_t( other)
{
    width_input_ = 0;
    height_input_ = 0;
    aspect_input_ = 0;
    menu_ = 0;
}

void image_format_param_t::set_value( const image::format_t& format, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    value().assign( format);
    emit_param_changed( reason);
}

param_t *image_format_param_t::do_clone() const { return new image_format_param_t( *this);}

void image_format_param_t::do_init()
{
    if( composition())
        value().assign( composition()->default_format());
    else
        value().assign( image::format_t());
}

void image_format_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    image::format_t format( get_value<image::format_t>( *this));
    hash_gen << format.width << "," << format.height << "," << format.aspect;
}

void image_format_param_t::do_read( const serialization::yaml_node_t& node)
{
    image::format_t format;
    node.get_value<image::format_t>( "value", format);
    set_value( format, silent_edit);
}

void image_format_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    image::format_t format( get_value<image::format_t>( *this));
    out << YAML::Key << "value" << YAML::Value << format;
}

QWidget *image_format_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();

    menu_ = new QComboBox( top);
    QSize s = menu_->sizeHint();

    QLabel *label = new QLabel( top);
    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    menu_->setFocusPolicy( Qt::NoFocus);

    for( int i = 0; i < image::format_t::presets().size(); ++i)
        menu_->addItem( image::format_t::presets()[i].first.c_str());

    menu_->addItem( "Custom");

    image::format_t format( get_value<image::format_t>( *this));

    menu_->move( app().ui()->inspector().left_margin(), 0);
    menu_->setCurrentIndex( format.preset_index());
    menu_->setEnabled( enabled());
    connect( menu_, SIGNAL( currentIndexChanged( int)), this, SLOT( preset_picked(int)));
    int h = s.height() + 5;

    width_input_ = new ui::double_spinbox_t( top);
    width_input_->setRange( 16, app().preferences().max_image_width());
    width_input_->setDecimals( 0);
    width_input_->setTrackMouse( false);
    width_input_->setValue( value().cast<image::format_t>().width);
    width_input_->move( app().ui()->inspector().left_margin(), h);
    width_input_->setMinimum( 16);
    width_input_->setValue( format.width);
    width_input_->setEnabled( enabled());
    connect( width_input_, SIGNAL( valueChanged( double)), this, SLOT( set_new_format( double)));
    s = width_input_->sizeHint();

    height_input_ = new ui::double_spinbox_t( top);
    height_input_->setRange( 16, app().preferences().max_image_height());
    height_input_->setDecimals( 0);
    height_input_->setValue( value().cast<image::format_t>().height);
    height_input_->setTrackMouse( false);
    height_input_->setEnabled( enabled());
    height_input_->move( app().ui()->inspector().left_margin() + s.width() + 5, h);
    height_input_->setMinimum( 16);
    height_input_->setValue( format.height);
    connect( height_input_, SIGNAL( valueChanged( double)), this, SLOT( set_new_format(double)));

    aspect_input_ = new ui::double_spinbox_t( top);
    aspect_input_->setValue( 1);
    aspect_input_->setTrackMouse( false);
    aspect_input_->setEnabled( enabled());
    aspect_input_->setValue( format.aspect);
    aspect_input_->setDecimals( 3);
    aspect_input_->setMinimum( 0.1);
    aspect_input_->setSingleStep( 0.05);
    aspect_input_->move( app().ui()->inspector().left_margin() + ( 2 * s.width()) + 10, h);
    connect( aspect_input_, SIGNAL( valueChanged( double)), this, SLOT( set_new_format(double)));
    h += s.height() + 5;

    int w = ( width_input_->sizeHint().width() * 2) + aspect_input_->sizeHint().width() + 10;
    menu_->resize( w, menu_->sizeHint().height());

    top->setMinimumSize( app().ui()->inspector().width(), h);
    top->setMaximumSize( app().ui()->inspector().width(), h);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void image_format_param_t::do_update_widgets()
{
    if( width_input_)
    {
        width_input_->blockSignals( true);
        height_input_->blockSignals( true);
        menu_->blockSignals( true);
        aspect_input_->blockSignals( true);

        image::format_t f( get_value<image::format_t>( *this));

        width_input_->setValue( f.width);
        height_input_->setValue( f.height);
        menu_->setCurrentIndex( f.preset_index());
        aspect_input_->setValue( f.aspect);

        width_input_->blockSignals( false);
        height_input_->blockSignals( false);
        menu_->blockSignals( false);
        aspect_input_->blockSignals( false);
    }
}

void image_format_param_t::do_enable_widgets( bool e)
{
    if( width_input_)
    {
        width_input_->setEnabled( e);
        height_input_->setEnabled( e);
        menu_->setEnabled( e);
        aspect_input_->setEnabled( e);
    }
}

void image_format_param_t::preset_picked( int v)
{
    if( v == image::format_t::presets().size())
        return;

    image::format_t new_format;
    new_format.set_from_preset( v);

    param_set()->begin_edit();
    set_value( new_format);
    param_set()->end_edit();
    update_widgets();
}

void image_format_param_t::set_new_format( double unused)
{
    int w = width_input_->value();
    int h = height_input_->value();
    float a = aspect_input_->value();

    image::format_t new_format( w, h, a);

    menu_->blockSignals( true);
    menu_->setCurrentIndex( new_format.preset_index());
    menu_->blockSignals( false);

    param_set()->begin_edit();
    set_value( new_format);
    param_set()->end_edit();
}

} // namespace
