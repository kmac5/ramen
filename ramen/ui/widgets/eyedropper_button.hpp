// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_EYEDROPPER_BUTTON_HPP
#define	RAMEN_UI_EYEDROPPER_BUTTON_HPP

#include<ramen/ui/widgets/eyedropper_button_fwd.hpp>

#include<QPushButton>

#include<ramen/ui/widgets/color.hpp>

namespace ramen
{
namespace ui
{

class eyedropper_button_t : public QPushButton
{
    Q_OBJECT
    
public:

    eyedropper_button_t( QWidget *parent = 0);

    bool eventFilter( QObject *watched, QEvent *event);

Q_SIGNALS:

    void color_picked( const ramen::ui::color_t&);

private Q_SLOTS:

    void tool_selected( bool b);

private:

    color_t value_;
};

} // namespace
} // namespace

#endif
