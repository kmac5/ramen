// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_PARAM_SPINBOX_HPP
#define RAMEN_UI_WIDGETS_PARAM_SPINBOX_HPP

#include<ramen/config.hpp>

#include<ramen/ui/widgets/param_spinbox_fwd.hpp>
#include<ramen/ui/widgets/spinbox.hpp>

#include<boost/variant.hpp>

#include<ramen/params/param_fwd.hpp>
#include<ramen/params/animated_param_fwd.hpp>

class QAction;

namespace ramen
{
	
namespace ui
{

class param_spinbox_t : public spinbox_t
{
    Q_OBJECT

public:

	typedef boost::variant<double, std::string> value_type;
	
	param_spinbox_t( param_t& param, int comp_index, QWidget *parent = 0);
	
	double value() const;
	std::string stringValue() const;
	
	bool setValue( double v);
	void setValue( const std::string& s);
	void restorePreviousValue();

protected:

	virtual void keyPressEvent( QKeyEvent *event);
	virtual void keyReleaseEvent( QKeyEvent *event);
	
	virtual void mousePressEvent( QMouseEvent *event);
	virtual void mouseMoveEvent( QMouseEvent *event);
	virtual void mouseReleaseEvent( QMouseEvent *event);
	
	virtual void contextMenuEvent( QContextMenuEvent *event);
	
Q_SIGNALS:

	void valueChanged( double value);
    void spinBoxPressed();
    void spinBoxDragged( double value);
    void spinBoxReleased();

private Q_SLOTS:

    void textChanged();
	
	void set_key();
	void copy_anim();
	void paste_anim();
	void delete_key();
	void delete_anim();
	
private:
	
	void setLineEditContents( const std::string& s);
	void stepBy( int steps);

	param_t& param_;
	animated_param_t *aparam_;
	int comp_index_;

	value_type value_;
	value_type previous_value_;

	QAction *set_key_, *del_key_;
	QAction *del_anim_, *copy_anim_;
	QAction *paste_anim_;
	
	QAction *del_expr_, *expr_editor_;
	QAction *copy_ref_, *paste_ref_;
};

} // ui
} // ramen

#endif
