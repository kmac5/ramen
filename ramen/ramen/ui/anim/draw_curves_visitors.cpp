// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/draw_curves_visitors.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<QPainter>

#include<ramen/anim/float_curve.hpp>
#include<ramen/anim/shape_curve.hpp>

#include<ramen/anim/fwd_diff_cubic_evaluator.hpp>

#include<ramen/ui/palette.hpp>
#include<ramen/ui/anim/anim_curves_view.hpp>

namespace ramen
{
namespace ui
{
namespace
{
	
void draw_span( const anim_curves_view_t& view, const anim::float_curve_t *c, 
				const anim::float_key_t& k0, const anim::float_key_t& k1, float lo, float hi)
{
    // trivial reject spans outside the view
    if( k0.time() > view.viewport().world().max.x)
        return;

    if( k1.time() < view.viewport().world().min.x)
        return;

    // handle step special case
    if( k0.v1_auto_tangent() == anim::keyframe_t::tangent_step)
    {
		float v0 = c->relative_to_absolute( k0.value());
		float v1 = c->relative_to_absolute( k1.value());
		
        view.painter()->drawLine( QPointF( k0.time(), v0), QPointF( k1.time(), v0));
        view.painter()->drawLine( QPointF( k1.time(), v0), QPointF( k1.time(), v1));
        return;
    }

    // linear segment optimization
    if( k0.v1_auto_tangent() == anim::keyframe_t::tangent_linear && k1.v0_auto_tangent() == anim::keyframe_t::tangent_linear)
    {
		float v0 = c->relative_to_absolute( k0.value());
		float v1 = c->relative_to_absolute( k1.value());
        view.painter()->drawLine( QPointF( k0.time(), v0), QPointF( k1.time(), v1));
        return;
    }

    int steps = view.span_num_steps( k0.time(), k1.time());

    float h0 = k0.time();
    float v0 = c->relative_to_absolute( k0.value());

    float h_inc = (k1.time() - h0) / (float) steps;
    float h = h0 + h_inc;

    float t_inc = 1.0f / (float) steps;
    float t = t_inc;

    anim::fwd_diff_cubic_evaluator_t<float> eval( k0.cubic_polynomial(), t_inc);
    ++eval; // advance the first value

    for( int i = 1; i <= steps; ++i)
    {
        float v = c->relative_to_absolute( adobe::clamp( eval(), lo, hi));
        view.painter()->drawLine( QPointF( h0, v0), QPointF( h, v));

        h0 = h;
        v0 = v;
        h += h_inc;
        t += t_inc;
        ++eval;
    }
}

void draw_keyframe( const anim_curves_view_t& view, const anim::float_curve_t *c, 
					const anim::float_key_t& k, const anim::float_key_t *prev,
					const anim::float_key_t *next, bool draw_tangents)
{
    QPen pen;

    if( k.selected())
        pen.setColor( QColor( 255, 0, 0));
    else
        pen.setColor( palette_t::instance().qcolor( "text"));

    view.painter()->setPen( pen);

    Imath::V2f p( k.time(), c->relative_to_absolute( k.value()));
    Imath::V2i q( view.world_to_screen( p));
    view.draw_small_box( q);

	if( draw_tangents)
	{
	    bool draw_left_tan;
	    bool draw_right_tan;

	    tangents_mask( k, prev, next, draw_left_tan, draw_right_tan);

	    if( draw_left_tan)
	    {
	        Imath::V2i qt( view.left_tangent_pos( q, k.v0(), c->scale()));
	        view.painter()->drawLine( QPoint( q.x, q.y), QPoint( qt.x, qt.y));
	        view.draw_small_box( qt);
	    }

	    if( draw_right_tan)
	    {
	        Imath::V2f qt( view.right_tangent_pos( q, k.v1(), c->scale()));
	        view.painter()->drawLine( QPoint( q.x, q.y), QPoint( qt.x, qt.y));
	        view.draw_small_box( qt);
	    }
	}
}

} // unnamed

void tangents_mask( const anim::float_key_t& k, const anim::float_key_t *prev, const anim::float_key_t *next, bool& left, bool& right)
{
    left = right = true;

    if( !prev)
        left = false;
    else
    {
        if( prev->v1_auto_tangent() == anim::float_key_t::tangent_step)
            left = false;
    }

    if( !next || ( k.v1_auto_tangent() == anim::float_key_t::tangent_step))
        right = false;
}

draw_curve_visitor::draw_curve_visitor( const anim_curves_view_t& view) : view_( view) {}

void draw_curve_visitor::operator()( const anim::float_curve_t *c)
{
    if( c->size() < 2)
        return;

    for( anim::float_curve_t::const_iterator it( c->begin()); it != (c->end()-1); ++it)
        draw_span( view_, c, *it, *(it+1), c->get_min(), c->get_max());

    // draw pre
    if( view_.viewport().world().min.x < c->start_time())
    {
        switch( c->extrapolation())
        {
        case anim::extrapolate_constant:
		{
			float v = c->relative_to_absolute( c->keys().front().value());
            view_.painter()->drawLine( QPointF( view_.viewport().world().min.x, v),
            QPointF( c->start_time(), v));
		}
        break;

		// TODO: check this
		case anim::extrapolate_linear:
		{
			float v = c->relative_to_absolute( c->keys().front().value());
			Imath::V2f p( c->start_time(), v);
	        Imath::V2f dir( -1, -c->keys().front().v1() * c->scale());
			dir.normalize();
			Imath::V2f q( view_.viewport().world().min.x, 0);
			float t = ( q.x - p.x) / dir.x;
			q.y = p.y + t * dir.y;
			
			float rmin = c->relative_to_absolute( c->get_min());
			if( q.y < rmin)
			{
				Imath::V2f mid( 0, rmin);
				float t = ( mid.y - p.y) / dir.y;
				mid.x = p.x + t * dir.x;
	            view_.painter()->drawLine( QPointF( p.x, p.y), QPointF( mid.x, mid.y));				
	            view_.painter()->drawLine( QPointF( mid.x, mid.y), QPointF( q.x, rmin));
			}
			else
	            view_.painter()->drawLine( QPointF( p.x, p.y), QPointF( q.x, q.y));
		}
		break;
		
        default:
        {
            float pixel_len = ( c->start_time() - view_.viewport().world().min.x) * view_.time_scale();
            int steps = adobe::clamp( (int) pixel_len / 4, 5, 150);

            float h0 = view_.viewport().world().min.x;
            float v0 = c->relative_to_absolute( c->evaluate( h0));

            float h_inc = ( c->start_time() - view_.viewport().world().min.x) / (float) steps;
            float h = h0 + h_inc;

            for( int i = 1; i <= steps; ++i)
            {
                float v = c->relative_to_absolute( c->evaluate( h));
                view_.painter()->drawLine( QPointF( h0, v0), QPointF( h, v));

                h0 = h;
                v0 = v;
                h += h_inc;
            }
        }
        break;
        }
    }

    // draw post
    if( view_.viewport().world().max.x > c->end_time())
    {
        switch( c->extrapolation())
        {
        case anim::extrapolate_constant:
		{
			float v = c->relative_to_absolute( c->keys().back().value());
            view_.painter()->drawLine( QPointF( c->end_time(), v), QPointF( view_.viewport().world().max.x, v));
		}
        break;

		// TODO: check this
		case anim::extrapolate_linear:
		{
			float v = c->relative_to_absolute( c->keys().back().value());
			Imath::V2f p( c->end_time(), v);
	        Imath::V2f dir( 1, c->keys().back().v0() * c->scale());
			dir.normalize();
			Imath::V2f q( view_.viewport().world().max.x, 0);
			float t = ( q.x - p.x) / dir.x;
			q.y = p.y + t * dir.y;
			
			float rmax = c->relative_to_absolute( c->get_max());
			if( q.y > rmax)
			{
				Imath::V2f mid( 0, rmax);
				float t = ( mid.y - p.y) / dir.y;
				mid.x = p.x + t * dir.x;
	            view_.painter()->drawLine( QPointF( p.x, p.y), QPointF( mid.x, mid.y));				
	            view_.painter()->drawLine( QPointF( mid.x, mid.y), QPointF( q.x, rmax));
			}
			else
	            view_.painter()->drawLine( QPointF( p.x, p.y), QPointF( q.x, q.y));
		}
		break;
		
        default:
        {
            float pixel_len = ( c->start_time() - view_.viewport().world().min.x) * view_.time_scale();
            int steps = adobe::clamp( (int) pixel_len / 4, 5, 300);

            float h0 = c->end_time();
            float v0 = c->relative_to_absolute( c->evaluate( h0));

            float h_inc = ( view_.viewport().world().max.x - c->end_time()) / (float) steps;
            float h = h0 + h_inc;

            for( int i = 1; i <= steps; ++i)
            {
                float v = c->relative_to_absolute( c->evaluate( h));
                view_.painter()->drawLine( QPointF( h0, v0), QPointF( h, v));

                h0 = h;
                v0 = v;
                h += h_inc;
            }
        }
        break;
        }
    }
}

void draw_curve_visitor::operator()( const anim::shape_curve2f_t *c)
{
	const anim::shape_curve2f_t& curve( *c);
	
    if( curve.size() < 2)
        return;

	for( int i = 0; i < curve.size()-1; ++i)
	{
		const anim::shape_key2f_t& k0( curve[i]);
		const anim::shape_key2f_t& k1( curve[i+1]);

		// trivial reject spans outside the view
		if( k0.time() > view_.viewport().world().max.x)
			continue;
	
		if( k1.time() < view_.viewport().world().min.x)
			continue;
	
		// handle step special case
		if( k0.v1_auto_tangent() == anim::keyframe_t::tangent_step)
		{
			view_.painter()->drawLine( QPointF( k0.time(), i), QPointF( k1.time(), i));
			view_.painter()->drawLine( QPointF( k1.time(), i), QPointF( k1.time(), i+1));
		}
		else
		{
			// linear segment optimization
			if( k0.v1_auto_tangent() == anim::keyframe_t::tangent_linear && k1.v0_auto_tangent() == anim::keyframe_t::tangent_linear)
				view_.painter()->drawLine( QPointF( k0.time(), i), QPointF( k1.time(), i+1));
			else
			{
				RAMEN_ASSERT( 0 && "Unknown tangent type in shape_curve2f_t");
			}
		}
	}
	
    // draw pre
    if( view_.viewport().world().min.x < c->start_time())
    {
        switch( c->extrapolation())
        {
	        case anim::extrapolate_constant:
			{
	            view_.painter()->drawLine( QPointF( view_.viewport().world().min.x, 0),
	            QPointF( c->start_time(), 0));
			}
	        break;
		
	        default:
				RAMEN_ASSERT( 0);
	        break;
        }
    }
	
	// draw post
    if( view_.viewport().world().max.x > c->end_time())
    {
        switch( c->extrapolation())
        {
	        case anim::extrapolate_constant:
	            view_.painter()->drawLine( QPointF( c->end_time(), c->size() - 1), 
										   QPointF( view_.viewport().world().max.x, c->size() - 1));
	        break;
			
			default:
				RAMEN_ASSERT( 0);
			break;
		}
	}
}

draw_keyframes_visitor::draw_keyframes_visitor( const anim_curves_view_t& view, bool draw_tangents) : view_( view)
{
	draw_tangents_ = draw_tangents;
}

void draw_keyframes_visitor::operator()( const anim::float_curve_t *c)
{
    const anim::float_curve_t& curve( *c);
	
    if( curve.empty())
        return;

    if( curve.size() == 1)
    {
        draw_keyframe( view_, c, curve[0], 0, 0, false);
        return;
    }

    draw_keyframe( view_, c, curve[0], 0, &(curve[1]), draw_tangents_);

    for( int i = 1; i < curve.size() - 1; ++i)
        draw_keyframe( view_, c, curve[i], &(curve[i-1]), &(curve[i+1]), draw_tangents_);

    draw_keyframe( view_, c, curve[ curve.size()-1], &(curve[ curve.size()-2]), 0, draw_tangents_);
}

void draw_keyframes_visitor::operator ()( const anim::shape_curve2f_t *c)
{
	const anim::shape_curve2f_t& curve( *c);

    if( curve.empty())
        return;

    for( int i = 0; i < curve.size(); ++i)
	{
		QPen pen;
	
		if( curve[i].selected())
			pen.setColor( QColor( 255, 0, 0));
		else
			pen.setColor( palette_t::instance().qcolor( "text"));

		view_.painter()->setPen( pen);
		
		Imath::V2f p( curve[i].time(), i);
		Imath::V2i q( view_.world_to_screen( p));
		view_.draw_small_box( q);
	}	
}

bbox_curve_visitor::bbox_curve_visitor( const Imath::Box2f box, bool sel_only) : bbox( box), selected_only( sel_only) {}

void bbox_curve_visitor::operator()( const anim::float_curve_t *c)
{
    for( anim::float_curve_t::const_iterator it( c->begin()); it != c->end(); ++it)
    {
        if( selected_only && !it->selected())
            continue;

        Imath::V2f v( it->time(), c->relative_to_absolute( it->value()));
        bbox.extendBy( v);
    }
}

void bbox_curve_visitor::operator ()( const anim::shape_curve2f_t *c)
{
	const anim::shape_curve2f_t& curve( *c);

    for( int i = 0; i < curve.size(); ++i)
	{
        if( selected_only && !curve[i].selected())
            continue;
		
		Imath::V2f v( curve[i].time(), i);
		bbox.extendBy( v);
	}	
}

} // namespace
} // namespace
