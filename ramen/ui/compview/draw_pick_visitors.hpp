// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_COMP_VIEW_DRAW_PICK_VISITORS_HPP
#define RAMEN_UI_COMP_VIEW_DRAW_PICK_VISITORS_HPP

#include<ramen/nodes/node_visitor.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/ui/compview/pick_result.hpp>

class QPainter;

namespace ramen
{
namespace ui
{

class composition_view_t;

struct input_location_node_visitor : public generic_node_visitor
{
    input_location_node_visitor( int i = 0);
    virtual void visit( node_t *n);

    int index;
    Imath::V2f loc;
};

struct output_location_node_visitor : public generic_node_visitor
{
    output_location_node_visitor( int i = 0);
    virtual void visit( node_t *n);

    int index;
    Imath::V2f loc;
};

struct size_node_visitor : public generic_node_visitor
{
	size_node_visitor();
    virtual void visit( node_t *n);

	Imath::V2i size;
};

struct draw_node_visitor : public generic_node_visitor
{
    draw_node_visitor( QPainter& painter);
    virtual void visit( node_t *n);

private:

    QPainter& painter_;
};

struct draw_edges_visitor : public generic_node_visitor
{
    draw_edges_visitor( const composition_view_t& view, QPainter& painter);
    virtual void visit( node_t *n);

private:

    const composition_view_t& view_;
    QPainter& painter_;
};

struct pick_node_visitor : public generic_node_visitor
{
    pick_node_visitor( const composition_view_t& view, const Imath::V2f& p, pick_result_t& result);
    virtual void visit( node_t *n);

private:

    const composition_view_t& view_;
    Imath::V2f p_;
    pick_result_t& result_;
};

struct box_pick_node_visitor : public generic_node_visitor
{
    box_pick_node_visitor( const Imath::Box2f& box);
    virtual void visit( node_t *n);

    bool result;

private:

    Imath::Box2f box_;
};

struct pick_edge_visitor : public generic_node_visitor
{
    pick_edge_visitor( const composition_view_t& view, const Imath::V2f& p);
    virtual void visit( node_t *n);

    node_t *src, *dst;
    int port;

private:

    Imath::V2f p_;
    const composition_view_t& view_;
};

} // ui
} // ramen

#endif
