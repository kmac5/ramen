// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QR_OCIO_DISPLAY_WIDGET_HPP
#define RAMEN_QR_OCIO_DISPLAY_WIDGET_HPP

#include<QWidget>

namespace ramen
{
namespace ui
{

class ocio_display_combo_t;
class ocio_view_combo_t;

class ocio_display_widget_t : public QWidget
{
	Q_OBJECT

public:

	ocio_display_widget_t( Qt::Orientation orient = Qt::Horizontal, QWidget *parent = 0);

private:

	ocio_display_combo_t *display_;
	ocio_view_combo_t *view_;
};

} // ui
} // ramen

#endif
