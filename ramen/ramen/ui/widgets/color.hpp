// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_COLOR_HPP
#define	RAMEN_UI_WIDGETS_COLOR_HPP

#include<QColor>

namespace ramen
{
namespace ui
{

class color_t
{
public:

    color_t();
    color_t( double r, double g, double b, double a = 1.0f);

    operator QColor();

    color_t& operator*=( double s);

    double red() const	    { return red_;}
    double green() const    { return green_;}
    double blue() const	    { return blue_;}
    double alpha() const    { return alpha_;}

    void set_red( double x)   { red_ = x;}
    void set_green( double x) { green_ = x;}
    void set_blue( double x)  { blue_ = x;}
    void set_alpha( double x) { alpha_ = x;}

    void apply_gamma( double g = 2.2);
    void clamp();

    void to_hsv( double& h, double& s, double& v) const;
    static color_t from_hsv( double h, double s, double v);

private:

    double red_, green_, blue_, alpha_;
};

} // ui
} // ramen

#endif
