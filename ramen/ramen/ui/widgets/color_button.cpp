// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/color_button.hpp>

namespace ramen
{
namespace ui
{

color_button_t::color_button_t( QWidget *parent) : QPushButton( parent), value_( 0, 0, 0, 0)
{
    update_button_color();
}

void color_button_t::set_value( const color_t& c)
{
    value_ = c;
    update_button_color();
    value_changed( value_);
}

void color_button_t::set_red( double x)
{
    value_.set_red( x);
    update_button_color();
    value_changed( value_);
}

void color_button_t::set_green( double x)
{
    value_.set_green( x);
    update_button_color();
    value_changed( value_);
}

void color_button_t::set_blue( double x)
{
    value_.set_blue( x);
    update_button_color();
    value_changed( value_);
}

void color_button_t::update_button_color()
{
    QString str = "* { background-color: rgb( %1, %2, %3) }";
    QColor col = value_;
    setStyleSheet( str.arg( col.red()).arg( col.green()).arg( col.blue()));
}

} // ui
} // ramen
