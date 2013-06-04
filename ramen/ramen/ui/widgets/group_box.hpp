// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_GROUP_BOX_HPP
#define RAMEN_UI_WIDGETS_GROUP_BOX_HPP

#include<QGroupBox>

namespace ramen
{
namespace ui
{

class group_box_t : public QGroupBox
{
    Q_OBJECT

public:

	group_box_t( QWidget *parent = 0);

protected:

	virtual void mousePressEvent( QMouseEvent *e);	
	virtual void mouseReleaseEvent( QMouseEvent *e);

private:
	
	void collapse( bool b);
	
	bool collapsed_;
	bool clicked_;
};

} // ui
} // ramen

#endif
