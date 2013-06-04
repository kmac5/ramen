// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_DOUBLE_SPINBOX_HPP
#define RAMEN_UI_WIDGETS_DOUBLE_SPINBOX_HPP

#include<ramen/ui/widgets/double_spinbox_fwd.hpp>
#include<ramen/ui/widgets/spinbox.hpp>

#include<string>

namespace ramen
{
namespace ui
{

class double_spinbox_t : public spinbox_t
{
    Q_OBJECT

public:
	
	double_spinbox_t( QWidget *parent = 0);
	
	void setSuffix( const QString& s);
	
	double value() const;	
	void restorePreviousValue();

public Q_SLOTS:

	bool setValue( double v);
	
Q_SIGNALS:

	void valueChanged( double value);
    void spinBoxPressed();
    void spinBoxDragged( double value);
    void spinBoxReleased();

protected:

	virtual void keyPressEvent( QKeyEvent *event);
	virtual void keyReleaseEvent( QKeyEvent *event);
	
	virtual void mousePressEvent( QMouseEvent *event);
	virtual void mouseMoveEvent( QMouseEvent *event);
	virtual void mouseReleaseEvent( QMouseEvent *event);
	
	virtual void contextMenuEvent( QContextMenuEvent *event);
	
private Q_SLOTS:

    void textChanged();
	
private:

	void stepBy( int steps);
		
	double value_;
	double previous_value_;
};

} // ui
} // ramen

#endif
