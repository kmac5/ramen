// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/group_box.hpp>

#include<QMouseEvent>

namespace ramen
{
namespace ui
{

group_box_t::group_box_t( QWidget *parent) : QGroupBox( parent), collapsed_( false), clicked_( false) {}

void group_box_t::mousePressEvent( QMouseEvent *e)
{
	if( e->button() == Qt::LeftButton && e->pos().y() <= 16)
		clicked_ = true;
	else
		QGroupBox::mousePressEvent( e);
}

void group_box_t::mouseReleaseEvent( QMouseEvent *e)
{
	if( e->button() == Qt::LeftButton && clicked_ && e->pos().y() <= 16)
	{
		collapse( !collapsed_);
		clicked_ = false;
	}
	else
		QGroupBox::mouseReleaseEvent( e);
}

void group_box_t::collapse( bool b)
{
	collapsed_ = b;
	
	if( collapsed_)
		setFixedSize( width(), 16);
	else
		setFixedSize( width(), sizeHint().height());
}

} // ui
} // ramen
 
