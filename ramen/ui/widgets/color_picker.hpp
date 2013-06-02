// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_COLORPICKER_HPP
#define	RAMEN_UI_WIDGETS_COLORPICKER_HPP

#include<QDialog>

#include<ramen/ui/widgets/color.hpp>

class QDoubleSpinBox;

namespace ramen
{
namespace ui
{

class saturation_value_picker_t;
class hue_picker_t;
class exposure_picker_t;

class color_picker_t : public QDialog
{
    Q_OBJECT

public:

    color_picker_t( QWidget *parent, const color_t& c);

    const color_t& color() const { return color_;}

Q_SIGNALS:

    void color_changed( const ramen::ui::color_t&);

public Q_SLOTS:

    void set_hue( double h);
    void set_exposure( double e);
    void set_saturation_value( double s, double v);

private:

    void update_color();
    
    color_t prev_color_;
    color_t color_;

    saturation_value_picker_t *sv_picker_;
    hue_picker_t *hue_picker_;
    exposure_picker_t *exp_picker_;
    QDoubleSpinBox *exp_spinbox_;
    QDoubleSpinBox *red_spinbox_;
    QDoubleSpinBox *green_spinbox_;
    QDoubleSpinBox *blue_spinbox_;
};

} // ui
} // ramen

#endif
