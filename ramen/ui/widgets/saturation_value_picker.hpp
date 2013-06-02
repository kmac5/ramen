// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_SAT_VALUE_PICKER_HPP
#define	RAMEN_UI_WIDGETS_SAT_VALUE_PICKER_HPP

#include<QWidget>
#include<QImage>

namespace ramen
{
namespace ui
{

class saturation_value_picker_t : public QWidget
{
    Q_OBJECT

public:

    saturation_value_picker_t( QWidget *parent = 0);

    QSize sizeHint() const;

    double hue() const		{ return hue_;}
    double saturation() const	{ return saturation_;}
    double value() const	{ return value_;}

Q_SIGNALS:

    void saturation_value_changed( double sat, double val);

public Q_SLOTS:

    void set_hue( double h);
    void set_saturation_value( double s, double v);

protected:

    void paintEvent( QPaintEvent *event);
    void mouseMoveEvent( QMouseEvent *event);
    void mousePressEvent( QMouseEvent *event);
    void mouseReleaseEvent( QMouseEvent *event);
    void resizeEvent( QResizeEvent *event);

private:

    void pick_sat_val( QMouseEvent *event);

    void update_background();

    QImage background_;
    bool valid_background_;

    double hue_;
    double saturation_;
    double value_;
};

} // ui
} // ramen

#endif
