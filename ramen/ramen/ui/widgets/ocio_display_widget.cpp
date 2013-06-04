// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/ocio_display_widget.hpp>
#include<QHBoxLayout>
#include<QVBoxLayout>

#include<ramen/ui/widgets/ocio_display_combo.hpp>
#include<ramen/ui/widgets/ocio_view_combo.hpp>

namespace ramen
{
namespace ui
{

ocio_display_widget_t::ocio_display_widget_t( Qt::Orientation orient, QWidget *parent) : QWidget( parent)
{
	QLayout *layout;

	if( orient == Qt::Horizontal)
		layout = new QHBoxLayout();
	else
		layout = new QVBoxLayout();

	display_ = new ocio_display_combo_t();
	layout->addWidget( display_);

	view_ = new ocio_view_combo_t();
	layout->addWidget( view_);

	setLayout( layout);
}

} // ui
} // ramen
