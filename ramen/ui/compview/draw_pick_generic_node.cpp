// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/compview/draw_pick_generic_node.hpp>

#include<QPainter>

#include<ramen/app/application.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/compview/composition_view.hpp>
#include<ramen/ui/palette.hpp>

namespace ramen
{
namespace ui
{
namespace
{

const int node_min_width = 90;
const int node_extra_width = 20;
const int shadow_offset = 3;
const int radius = 4;

} // unnamed

Imath::V2f generic_input_location( const node_t *n, std::size_t i)
{
    float off = generic_node_width( n) / ( n->num_inputs() + 1);
    return Imath::V2f( n->location().x + (i+1)*off, n->location().y - 1);
}

Imath::V2f generic_output_location( const node_t *n)
{
    return Imath::V2f( n->location().x + generic_node_width( n) / 2, n->location().y + generic_node_height() + 2);
}

int generic_node_width( const node_t *n)
{
    return node_min_width + node_extra_width * std::max( (int) n->num_inputs() - 3, 0);
}

int generic_node_height() { return 20;}

void draw_generic_node( QPainter& painter, const node_t *n)
{
    Imath::V2f p = n->location();

    QPen pen;
    QBrush brush;

    brush.setStyle( Qt::SolidPattern);
    brush.setColor( palette_t::instance().qcolor( "node_shadow"));

    painter.setPen( Qt::NoPen);
    painter.setBrush( brush);
    painter.drawRoundedRect( QRectF( p.x + shadow_offset, p.y + shadow_offset, generic_node_width( n), generic_node_height()), radius, radius);

    for( unsigned int i=0;i<n->num_inputs();++i)
    {
        brush.setColor( QColor( n->input_plugs()[i].color().x, n->input_plugs()[i].color().y, n->input_plugs()[i].color().z));
        painter.setBrush( brush);
        Imath::V2f q = generic_input_location( n, i);
        painter.drawRect( q.x - 3, q.y - 3, 6, 6);
    }

    if( n->has_output_plug())
    {
        // TODO: use the output plug's color here.
        brush.setColor( palette_t::instance().qcolor( "out plug"));
        painter.setBrush( brush);
        Imath::V2f q = generic_output_location( n);
        painter.drawRect( q.x - 3, q.y - 3, 6, 6);
    }

    if( n->selected())
		brush.setColor( palette_t::instance().qcolor( "node_picked_bg"));
    else
    {
        if( n->plugin_error())
			brush.setColor( palette_t::instance().qcolor( "node_error_bg"));
        else
			brush.setColor( palette_t::instance().qcolor( "node_bg"));
    }

    painter.setBrush( brush);
    painter.drawRoundedRect( QRectF( p.x, p.y, generic_node_width( n), generic_node_height()), radius, radius);

    painter.setBrush( Qt::NoBrush);

    if( app().ui()->active_node() == n)
		pen.setColor( palette_t::instance().qcolor( "node_active_out"));
    else
    {
        if( app().ui()->context_node() == n)
			pen.setColor( palette_t::instance().qcolor( "node_context_out"));
        else
			pen.setColor( palette_t::instance().qcolor( "text"));
    }

    painter.setPen( pen);
    painter.drawRoundedRect( QRectF( p.x, p.y, generic_node_width( n), generic_node_height()), radius, radius);

    pen.setColor( palette_t::instance().qcolor( "text"));
    painter.setPen( pen);
    painter.drawText( QRectF( p.x, p.y + 3, generic_node_width( n), generic_node_height() - 3), Qt::AlignCenter, QString( n->name().c_str()));

    if( n->ignored())
    {
		pen.setColor( palette_t::instance().qcolor( "box_pick"));
        pen.setWidth(2);
        painter.setPen( pen);
        painter.drawLine( QPointF( p.x - 3, p.y - 2), QPointF( p.x + generic_node_width( n) + 3, p.y + generic_node_height() + 2));
    }
}

bool box_pick_generic_node( const node_t *n, const Imath::Box2f& b)
{
    if( n->location().x + generic_node_width( n) < b.min.x)
        return false;

    if( n->location().x > b.max.x)
        return false;

    if( n->location().y > b.max.y)
        return false;

    if( n->location().y + generic_node_height() < b.min.y)
        return false;

    return true;
}

void pick_generic_node( node_t *n, const Imath::V2f& p, const composition_view_t& view, pick_result_t& result)
{
    result.node = 0;
    result.component = pick_result_t::no_pick;
    result.plug_num = -1;

    if( ( p.x < n->location().x) || (p.x > n->location().x + generic_node_width( n)))
        return;

    if( ( p.y < n->location().y - 6.0) || ( p.y > n->location().y + generic_node_height() + 5))
        return;

    result.node = n;
    result.component = pick_result_t::body_picked;

    Imath::V2f q( generic_output_location( n));

    Imath::V2i pi( view.world_to_screen( p));
    Imath::V2i qi( view.world_to_screen( q));

    if( ( pi - qi).length2() < 25)
    {
        result.component = pick_result_t::output_picked;
        result.plug_num = 0;
        return;
    }

    for( unsigned int i=0;i<n->num_inputs();++i)
    {
        q = generic_input_location( n, i);
        qi = view.world_to_screen( q);

        if( ( pi - qi).length2() < 25)
        {
            result.component = pick_result_t::input_picked;
            result.plug_num = i;
            return;
        }
    }
}

} // namespace
} // namespace
