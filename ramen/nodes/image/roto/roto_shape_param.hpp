// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_SHAPE_PARAM_HPP
#define	RAMEN_ROTO_SHAPE_PARAM_HPP

#include<ramen/config.hpp>

#include<ramen/params/static_param.hpp>

#include<QPointer>

#include<ramen/nodes/image/roto/roto_node_fwd.hpp>
#include<ramen/nodes/image/roto/shape_fwd.hpp>

#include<ramen/ui/widgets/line_edit_fwd.hpp>
#include<ramen/ui/widgets/color_button_fwd.hpp>

class QComboBox;
class QToolButton;
class QPushButton;

namespace ramen
{

class roto_shape_param_t : public static_param_t
{
    Q_OBJECT

public:

    roto_shape_param_t( const std::string& id);
	
	void set_active_shape( roto::shape_t *s);

	void shape_list_changed();

protected:

    roto_shape_param_t( const roto_shape_param_t& other);
    void operator=( const roto_shape_param_t& other);

private Q_SLOTS:

    void rename_shape();	
	void set_autokey( bool b);
	void set_shape_key();
    void change_shape_color();
    void set_shape_parent( int index);
	void move_shape_order_up();
	void move_shape_order_down();

private:

    virtual param_t *do_clone() const;

    virtual QWidget *do_create_widgets();
	
	const image::roto_node_t& roto_node() const;
	image::roto_node_t& roto_node();

	void update_parent_menu();
	
	QPointer<ui::line_edit_t> name_input_;
	QPointer<QComboBox> parent_menu_;
	QPointer<ui::color_button_t> display_color_;
	QPointer<QToolButton> autokey_;
	QPointer<QPushButton> shape_key_;
	QPointer<QToolButton> order_up_;
	QPointer<QToolButton> order_down_;
	roto::shape_t *selected_;
};

} // namespace

#endif
