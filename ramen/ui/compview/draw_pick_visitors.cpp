// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/compview/draw_pick_visitors.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/ui/compview/composition_view.hpp>
#include<ramen/ui/compview/draw_pick_generic_node.hpp>

namespace ramen
{
namespace ui
{

input_location_node_visitor::input_location_node_visitor( int i) : index( i) {}
void input_location_node_visitor::visit( node_t *n)
{
    RAMEN_ASSERT( index >= 0 && index < n->num_inputs());
    loc = generic_input_location( n, index);
}

output_location_node_visitor::output_location_node_visitor( int i) : index( i) {}
void output_location_node_visitor::visit( node_t *n)
{
    RAMEN_ASSERT( index == 0);
    loc = generic_output_location( n);
}

size_node_visitor::size_node_visitor() {}
void size_node_visitor::visit( node_t *n)
{
	size = Imath::V2i( generic_node_width( n), generic_node_height());
}

draw_node_visitor::draw_node_visitor( QPainter& painter) : painter_( painter) {}
void draw_node_visitor::visit( node_t *n)
{
    draw_generic_node( painter_, n);
}

draw_edges_visitor::draw_edges_visitor( const composition_view_t& view, QPainter& painter) : view_( view), painter_( painter) {}

void draw_edges_visitor::visit( node_t *n)
{
    Imath::V2f p0( generic_output_location( n));
    input_location_node_visitor visitor;

    for( int i = 0; i < n->num_outputs(); ++i)
    {
        node_t *dst = boost::get<0>( n->output_plug().connections()[i]);
        visitor.index = boost::get<2>( n->output_plug().connections()[i]);
        dst->accept( visitor);
        view_.draw_bezier_edge( painter_, p0, visitor.loc);
    }
}

pick_node_visitor::pick_node_visitor( const composition_view_t& view, const Imath::V2f& p,
                                        pick_result_t& result) : view_( view), p_( p), result_( result) {}

void pick_node_visitor::visit( node_t *n)
{
    pick_generic_node( n, p_, view_, result_);
}

box_pick_node_visitor::box_pick_node_visitor( const Imath::Box2f& box) : box_( box), result( false) {}
void box_pick_node_visitor::visit( node_t *n)
{
    result = box_pick_generic_node( n, box_);
}

pick_edge_visitor::pick_edge_visitor( const composition_view_t& view, const Imath::V2f& p) : view_( view), src( 0), dst( 0), port( -1), p_( p)
{
}

void pick_edge_visitor::visit( node_t *n)
{
    Imath::V2f p0( generic_output_location( n));
    input_location_node_visitor visitor;

    for( int i = 0; i < n->num_outputs(); ++i)
    {
        node_t *target = boost::get<0>( n->output_plug().connections()[i]);
        visitor.index = boost::get<2>( n->output_plug().connections()[i]);
        target->accept( visitor);

        if( view_.pick_bezier_edge( p0, visitor.loc, p_))
        {
            src = n;
            dst = target;
            port = visitor.index;
            return;
        }
    }
}

} // ui
} // ramen
