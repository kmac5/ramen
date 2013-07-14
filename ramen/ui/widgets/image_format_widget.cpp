// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/image_format_widget.hpp>

#include<QLabel>
#include<QComboBox>
#include<QGridLayout>
#include<QSpinBox>
#include<QDoubleSpinBox>

namespace ramen
{
namespace ui
{

image_format_widget_t::image_format_widget_t( QWidget *parent) : QWidget( parent)
{
    QGridLayout *format_layout = new QGridLayout( this);
    format_layout->setContentsMargins( 0, 0, 0, 0);

    QLabel *label = new QLabel( "Presets");
    label->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    format_layout->addWidget( label, 0, 0, 1, 1);

    format_combo_ = new QComboBox();
    format_layout->addWidget( format_combo_, 0, 1, 1, 2);

    for( int i = 0; i < image::format_t::presets().size(); ++i)
        format_combo_->addItem( image::format_t::presets()[i].first.c_str());

    format_combo_->addItem( "Custom");
    connect( format_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( preset_picked( int)));

    label = new QLabel( "Image Size");
    label->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    format_layout->addWidget( label, 1, 0, 1, 1);

    image::format_t format = image::format_t::presets()[0].second;

    format_width_ = new QSpinBox();
    format_width_->setMinimum( 32);
    format_width_->setMaximum( 8192);
    format_width_->setValue( format.width);
    format_width_->setSuffix( " px");
    connect( format_width_, SIGNAL( valueChanged( int)), this, SLOT( width_changed( int)));
    format_layout->addWidget( format_width_, 1, 1, 1, 1);

    format_height_ = new QSpinBox();
    format_height_->setMinimum( 32);
    format_height_->setMaximum( 8192);
    format_height_->setValue( format.height);
    format_height_->setSuffix( " px");
    connect( format_height_, SIGNAL( valueChanged( int)), this, SLOT( height_changed( int)));
    format_layout->addWidget( format_height_, 1, 2, 1, 1);

    label = new QLabel( "Aspect");
    label->setAlignment( Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    format_layout->addWidget( label, 2, 1, 1, 1);

    format_aspect_ = new QDoubleSpinBox();
    format_aspect_->setRange( 0.1, 4);
    format_aspect_->setValue( format.aspect);
    format_aspect_->setSingleStep( 0.05);
    connect( format_aspect_, SIGNAL( valueChanged( double)), this, SLOT( aspect_changed( double)));
    format_layout->addWidget( format_aspect_, 2, 2, 1, 1);
}

image::format_t image_format_widget_t::value() const
{
    return image::format_t( format_width_->value(),
                            format_height_->value(),
                            format_aspect_->value());
}

void image_format_widget_t::set_value( const image::format_t& format)
{
    format_width_->blockSignals( true);
    format_height_->blockSignals( true);
    format_aspect_->blockSignals( true);

    format_width_->setValue( format.width);
    format_height_->setValue( format.height);
    format_aspect_->setValue( format.aspect);

    format_width_->blockSignals( false);
    format_height_->blockSignals( false);
    format_aspect_->blockSignals( false);
}

void image_format_widget_t::width_changed( int x)
{
    update_presets_menu();
    value_changed();
}

void image_format_widget_t::height_changed( int x)
{
    update_presets_menu();
    value_changed();
}

void image_format_widget_t::aspect_changed( double x)
{
    update_presets_menu();
    value_changed();
}

void image_format_widget_t::preset_picked( int x)
{
    if( x == image::format_t::presets().size()) // custom
        return;

    image::format_t format = image::format_t::presets()[x].second;
    set_value( format);
}

void image_format_widget_t::update_presets_menu()
{
    format_combo_->blockSignals( true);
    format_combo_->setCurrentIndex( value().preset_index());
    format_combo_->blockSignals( false);
}

} // ui
} // ramen
