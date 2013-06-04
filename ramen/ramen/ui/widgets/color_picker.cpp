// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/color_picker.hpp>

#include<cmath>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QDialogButtonBox>
#include<QLabel>
#include<QDoubleSpinBox>
#include<QLabel>

#include<ramen/ui/widgets/saturation_value_picker.hpp>
#include<ramen/ui/widgets/hue_picker.hpp>
#include<ramen/ui/widgets/exposure_picker.hpp>
#include<ramen/ui/widgets/color_swatch.hpp>

namespace ramen
{
namespace ui
{

color_picker_t::color_picker_t( QWidget *parent, const color_t& c) : QDialog( parent)
{
    prev_color_ = c;
    color_ = c;

    double h, s, v, e;
    color_.to_hsv( h, s, v);

    for( int i = 0; i < exposure_picker_t::max_exposure(); ++i)
    {
        double ex = std::pow( 2.0, (double) i);
        double x = v / ex;

        if( x <= 1.0)
        {
            e = i;
            v = x;
            break;
        }
    }

    setWindowTitle( tr( "Color Picker"));

    QVBoxLayout *vlayout = new QVBoxLayout( this);

    QHBoxLayout *hlayout = new QHBoxLayout();

    sv_picker_ = new saturation_value_picker_t();
    sv_picker_->set_hue( h);
    sv_picker_->set_saturation_value( s, v);
    connect( sv_picker_, SIGNAL( saturation_value_changed( double, double)), this, SLOT( set_saturation_value( double, double)));
    hlayout->addWidget( sv_picker_);

    hue_picker_ = new hue_picker_t();
    hue_picker_->set_hue( h);
    connect( hue_picker_, SIGNAL( hue_changed( double)), sv_picker_, SLOT( set_hue( double)));
    connect( hue_picker_, SIGNAL( hue_changed( double)), this, SLOT( set_hue( double)));
    hlayout->addWidget( hue_picker_);

    // color swatches
    {
        QVBoxLayout *vlayout2 = new QVBoxLayout();

        color_swatch_t *old_color_ = new color_swatch_t( 0, prev_color_);
        vlayout2->addWidget( old_color_);

        color_swatch_t *new_color_ = new color_swatch_t( 0, color_);
        connect( this, SIGNAL( color_changed( const ramen::ui::color_t&)), new_color_, SLOT( set_color( const ramen::ui::color_t&)));
        vlayout2->addWidget( new_color_);

        hlayout->addLayout( vlayout2);
    }

    vlayout->addLayout( hlayout);

    // exposure slider
    hlayout = new QHBoxLayout();

    QLabel *label = new QLabel( tr( "Exp"));
    hlayout->addWidget( label);

    exp_picker_ = new exposure_picker_t();
    exp_picker_->set_exposure( e);
    hlayout->addWidget( exp_picker_);

    exp_spinbox_ = new QDoubleSpinBox();
    exp_spinbox_->setMinimum( -20);
    exp_spinbox_->setMaximum(  20);
    exp_spinbox_->setValue( e);
    hlayout->addWidget( exp_spinbox_);

    connect( exp_spinbox_, SIGNAL( valueChanged( double)), exp_picker_, SLOT( set_exposure( double)));
    connect( exp_picker_,  SIGNAL( exposure_changed( double)), exp_spinbox_, SLOT( setValue( double)));
    connect( exp_spinbox_, SIGNAL( valueChanged( double)), this, SLOT( set_exposure( double)));
    vlayout->addLayout( hlayout);

    // dialog buttons
    hlayout = new QHBoxLayout();

    QSpacerItem *horizontal_spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hlayout->addItem(horizontal_spacer);

    // button rows
    QDialogButtonBox *button_box = new QDialogButtonBox();
    QSizePolicy sizePolicy1( QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth( button_box->sizePolicy().hasHeightForWidth());
    button_box->setSizePolicy( sizePolicy1);
    button_box->setOrientation( Qt::Horizontal);
    button_box->setStandardButtons( QDialogButtonBox::Cancel| QDialogButtonBox::Ok);
    button_box->setCenterButtons( false);
    hlayout->addWidget( button_box);

    connect( button_box, SIGNAL( accepted()), this, SLOT( accept()));
    connect( button_box, SIGNAL( rejected()), this, SLOT( reject()));

    vlayout->addLayout( hlayout);
}

void color_picker_t::set_hue( double h)                          { update_color();}
void color_picker_t::set_exposure( double e)                     { update_color();}
void color_picker_t::set_saturation_value( double s, double v)   { update_color();}

void color_picker_t::update_color()
{
    double e = std::pow( 2, exp_picker_->exposure());
    color_ = color_t::from_hsv( sv_picker_->hue(), sv_picker_->saturation(), sv_picker_->value() * e);
    color_changed( color_);
}

} // ui
} // ramen
