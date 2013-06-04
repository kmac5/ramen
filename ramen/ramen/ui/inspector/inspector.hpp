// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_INSPECTOR_HPP
#define	RAMEN_UI_INSPECTOR_HPP

#include<ramen/ui/inspector/inspector_fwd.hpp>

#include<boost/noncopyable.hpp>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/ui/inspector/panel_factory.hpp>

#include<ramen/ui/widgets/container_widget.hpp>
#include<ramen/ui/widgets/line_edit.hpp>

class QScrollArea;
class QPushButton;

namespace ramen
{
namespace ui
{

class inspector_t : public QObject
{
    Q_OBJECT

public:

    inspector_t();
    ~inspector_t();

    // metrics
    int left_margin() const;
    int width() const;

    void edit_node( node_t *n);

    void update();

    QWidget *widget() { return window_;}

	panel_factory_t& panel_factory() { return factory_;}

private Q_SLOTS:

    void rename_node();
	void show_help();
	
private:

    inspector_t( const inspector_t& other);
    void operator=( const inspector_t& other);

    void create_header();

    void recreate_panel( node_t *n);

    void update_header_widgets();

    // metrics
    mutable int left_margin_;
    mutable int width_;

    QWidget *window_;
    QScrollArea *scroll_;

    QWidget *header_;
    ui::line_edit_t *name_edit_;
	QPushButton *help_;

    container_widget_t *view_;

	panel_factory_t factory_;
    panel_factory_t::iterator current_;
};

} // namespace
} // namespace

#endif
