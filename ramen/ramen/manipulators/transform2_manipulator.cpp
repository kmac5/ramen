// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/manipulators/transform2_manipulator.hpp>

#include<ramen/assert.hpp>

#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/nodes/image_node.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ui/viewer/viewer_strategy.hpp>
#include<ramen/ui/palette.hpp>

namespace ramen
{

transform2_manipulator_t::transform2_manipulator_t( param_t *param) : manipulator_t()
{
	param_ = dynamic_cast<transform2_param_t*>( param);
	RAMEN_ASSERT( param_);
	
	drag_center_ = drag_axes_ = false;
	dragx_ = dragy_ = true;
}

void transform2_manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
	const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
	RAMEN_ASSERT( node);
	
	Imath::M33d m( param_->matrix_at_frame( app().document().composition().frame(), node->aspect_ratio()));

    // adjust for the aspect factor
    Imath::M33d mm = m * Imath::M33d().setScale( Imath::V2d( event.aspect_ratio, 1));

    Imath::V2f p = get_absolute_value<Imath::V2f>( param_->center_param());
    p = p * mm;

	gl_line_width( default_line_width());
    manipulators::draw_xy_axes( p, 70 / event.pixel_scale, 70 / event.pixel_scale,
								get_value<float>( param_->rotate_param()), 
								event.aspect_ratio / node->aspect_ratio(),
								  ui::palette_t::instance().color( "x_axis"),
								  ui::palette_t::instance().color( "y_axis"), event.pixel_scale);

    manipulators::draw_cross( p, 3 / event.pixel_scale, 3 / event.pixel_scale, default_color(), event.pixel_scale);
	manipulators::draw_ellipse( p, 7 / event.pixel_scale, 7 / event.pixel_scale, Imath::Color3c( 255, 255, 255), 20);
	 
    // draw the boundary
    Imath::Box2i bbox( node->format());
	++bbox.max.x;
	++bbox.max.y;

	float offset = manipulators::shadow_offset( event.pixel_scale);
	gl_line_width( manipulator_t::default_line_width());
	gl_point_size( manipulator_t::default_control_point_size());
	
	// shadow
	gl_color3ub( 0, 0, 0);
    gl_begin( GL_LINE_LOOP);
		manipulators::gl_transformed_box( bbox, mm, offset);
    gl_end();
	
	// color	
	gl_color( manipulator_t::default_color());
    gl_begin( GL_LINE_LOOP);
		manipulators::gl_transformed_box( bbox, mm);
    gl_end();
}

bool transform2_manipulator_t::do_key_press_event( const ui::key_press_event_t& event) { return false;}
void transform2_manipulator_t::do_key_release_event( const ui::key_release_event_t& event) {}

bool transform2_manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{ 
	drag_center_ = drag_axes_ = false;
	dragx_ = dragy_ = true;
	
	const image_node_t *node = dynamic_cast<const image_node_t*>( parent());
	RAMEN_ASSERT( node);
	
	float aspect = node->aspect_ratio();
	Imath::M33d m( param_->matrix_at_frame( app().document().composition().frame(), aspect));

    // adjust for the aspect factor
    Imath::M33d mm = m * Imath::M33d().setScale( Imath::V2d( event.aspect_ratio, 1));

    Imath::V2f c = get_absolute_value<Imath::V2f>( param_->center_param());
    c = c * mm;

	switch( manipulators::pick_xy_axes( event.wpos, c, 70 / event.pixel_scale, 70 / event.pixel_scale,
										get_value<float>( param_->rotate_param()),
										event.aspect_ratio / aspect, event.pixel_scale))
	{
		case manipulators::axes_center_picked:
		{
			if( event.modifiers & ui::event_t::control_modifier)
				drag_center_ = true;
			else
				drag_axes_ = true;
		}
		return true;
		
		case manipulators::axes_x_picked:
		{
			dragy_ = false;
			
			if( event.modifiers & ui::event_t::control_modifier)
				drag_center_ = true;
			else
				drag_axes_ = true;
		}
		return true;
	
		case manipulators::axes_y_picked:
		{
			dragx_ = false;
			
			if( event.modifiers & ui::event_t::control_modifier)
				drag_center_ = true;
			else
				drag_axes_ = true;
		}
		return true;
	
		default:
			return false;
	}
}

void transform2_manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
	RAMEN_ASSERT( drag_axes_ || drag_center_);

	if( event.first_drag)
		param_->param_set()->begin_edit();

	Imath::V2f off( event.wpos - event.last_wpos);
	off.x /= event.aspect_ratio;
	
	if( drag_center_)
		param_->move_center( off);
	else
		param_->translate( off);

	if( param_->track_mouse())
		param_->param_set()->notify_parent();
	else
		event.view->update();
}

void transform2_manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
	if( event.dragged)
	{
		RAMEN_ASSERT( drag_axes_ || drag_center_);
		param_->param_set()->end_edit( param_->track_mouse());
	}
	
	drag_center_ = drag_axes_ = false;
	dragx_ = dragy_ = true;
}

} // namespace
