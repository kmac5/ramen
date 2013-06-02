// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_COLORBUTTON_HPP
#define	RAMEN_UI_WIDGETS_COLORBUTTON_HPP

#include<ramen/ui/widgets/color_button_fwd.hpp>

#include<QPushButton>

#include<ramen/ui/widgets/color.hpp>

namespace ramen
{
namespace ui
{

class color_button_t : public QPushButton
{
    Q_OBJECT

public:

    color_button_t( QWidget *parent = 0);

    const color_t& value() const { return value_;}

public Q_SLOTS:

    void set_value( const color_t& c);

    void set_red( double x);
    void set_green( double x);
    void set_blue( double x);

Q_SIGNALS:

    void value_changed( const ramen::ui::color_t&);

private:

    void update_button_color();

    double clamp( double x) const
    {
        if( x < 0)
            return 0;

        if( x > 1)
            return 1;

        return x;
    }

    color_t value_;
};

} // ui
} // ramen

#endif
