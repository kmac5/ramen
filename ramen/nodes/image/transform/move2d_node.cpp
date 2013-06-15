// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/transform/move2d_node.hpp>

#include<boost/bind.hpp>

#include<ramen/assert.hpp>

#include<ramen/params/numeric_param.hpp>
#include<ramen/params/transform2_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/button_param.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/manipulators/manipulator.hpp>
#include<ramen/manipulators/draw.hpp>
#include<ramen/manipulators/pick.hpp>

#include<ramen/GL/Imath_gl.hpp>

#include<ramen/ui/viewer/viewer_strategy.hpp>

namespace ramen
{
namespace image
{

class move2d_manipulator_t : public manipulator_t
{
public:

	move2d_manipulator_t( param_t *param) : node_( 0)
	{
		param_ = dynamic_cast<transform2_param_t*>( param);
		RAMEN_ASSERT( param_);

		drag_center_ = drag_axes_ = false;
		dragx_ = dragy_ = true;
	}

private:

	virtual void do_draw_overlay( const ui::paint_event_t& event) const
	{
		RAMEN_ASSERT( node()->composition());

		move2d_node_t::matrix3_type lm( param_->matrix_at_frame( node()->composition()->frame(), node()->aspect_ratio()));

		Imath::V2f p = get_absolute_value<Imath::V2f>( param_->center_param());
	    p = p * lm;
		p.x *= node()->aspect_ratio();

		gl_line_width( default_line_width());
	    manipulators::draw_xy_axes( p, 70 / event.pixel_scale, 70 / event.pixel_scale,
									get_value<float>( param_->rotate_param()),
									event.aspect_ratio / node()->aspect_ratio(),
									  ui::palette_t::instance().color("x_axis"),
									  ui::palette_t::instance().color("y_axis"), event.pixel_scale);

	    manipulators::draw_cross( p, 3 / event.pixel_scale, 3 / event.pixel_scale, default_color(), event.pixel_scale);
		manipulators::draw_ellipse( p, 7 / event.pixel_scale, 7 / event.pixel_scale, Imath::Color3c( 255, 255, 255), 20);

		// draw the boundary
		move2d_node_t::matrix3_type gm( node()->global_matrix());
		gm *= move2d_node_t::matrix3_type().setScale( move2d_node_t::vector2_type( node()->aspect_ratio(), 1));

	    Imath::Box2i bbox( node()->format());
		++bbox.max.x;
		++bbox.max.y;

		float offset = manipulators::shadow_offset( event.pixel_scale);
		gl_line_width( manipulator_t::default_line_width());
		gl_point_size( manipulator_t::default_control_point_size());

		// shadow
		gl_color3ub( 0, 0, 0);
	    gl_begin( GL_LINE_LOOP);
			manipulators::gl_transformed_box( bbox, gm, offset);
	    gl_end();

		// color
		gl_color( manipulator_t::default_color());
	    gl_begin( GL_LINE_LOOP);
			manipulators::gl_transformed_box( bbox, gm);
	    gl_end();
	}

	virtual bool do_key_press_event( const ui::key_press_event_t& event) { return false;}
	virtual void do_key_release_event( const ui::key_release_event_t& event) {}

	virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event)
	{
		RAMEN_ASSERT( node()->composition());

		drag_center_ = drag_axes_ = false;
		dragx_ = dragy_ = true;

		move2d_node_t::matrix3_type m( param_->matrix_at_frame( node()->composition()->frame(), node()->aspect_ratio()));

	    // adjust for the aspect factor
	    Imath::V2f c = get_absolute_value<Imath::V2f>( param_->center_param());
	    c = c * m;
		c.x *= node()->aspect_ratio();

		switch( manipulators::pick_xy_axes( event.wpos, c, 70 / event.pixel_scale, 70 / event.pixel_scale,
											get_value<float>( param_->rotate_param()),
											event.aspect_ratio / node()->aspect_ratio(), event.pixel_scale))
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

	virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event)
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

	virtual void do_mouse_release_event( const ui::mouse_release_event_t& event)
	{
		if( event.dragged)
		{
			RAMEN_ASSERT( drag_axes_ || drag_center_);
			param_->param_set()->end_edit( param_->track_mouse());
		}

		drag_center_ = drag_axes_ = false;
		dragx_ = dragy_ = true;
	}

	const move2d_node_t *node() const
	{
		if( !node_)
			get_node();

		return node_;
	}

	move2d_node_t *node()
	{
		if( !node_)
			get_node();

		return node_;
	}

	void get_node() const
	{
		const move2d_node_t *n = dynamic_cast<const move2d_node_t*>( parent());
		RAMEN_ASSERT( n);
		node_ = const_cast<move2d_node_t*>( n);
	}

	mutable move2d_node_t *node_;
	transform2_param_t *param_;
	bool drag_center_;
	bool drag_axes_;
	bool dragx_, dragy_;
};

move2d_node_t::move2d_node_t() : xform2d_node_t()
{ 
	set_name("move2d");
    param_set().param_changed.connect( boost::bind( &move2d_node_t::param_changed, this, _1, _2));
}

move2d_node_t::move2d_node_t( const move2d_node_t& other) : xform2d_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &move2d_node_t::param_changed, this, _1, _2));
}

void move2d_node_t::do_create_params()
{
    std::auto_ptr<transform2_param_t> xp( new transform2_param_t( "xform", "xf"));
    add_param( xp);

	create_default_filter_param();

    std::auto_ptr<button_param_t> b( new button_param_t( "Apply Track..."));
    b->set_id( "apply_track");
    add_param( b);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

	create_motion_blur_param();
}

void move2d_node_t::do_create_manipulators()
{	
    std::auto_ptr<move2d_manipulator_t> m( new move2d_manipulator_t( &param( "xf")));
    add_manipulator( m);
}

move2d_node_t::matrix3_type move2d_node_t::do_calc_transform_matrix_at_frame( float frame, int subsample) const
{
    const transform2_param_t *p = dynamic_cast<const transform2_param_t*>( &param( "xf"));
    RAMEN_ASSERT( p);
    return p->matrix_at_frame( frame, aspect_ratio(), subsample);
}

void move2d_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	RAMEN_ASSERT( composition());
	
	if( reason == param_t::user_edited && p == &param( "apply_track"))
	{
        /*
		apply_track_mode mode;
		apply_track_use use;
		
		const tracker_node_t *tracker = ui::apply_track_dialog_t::instance().exec( mode,  use);
		
		if( tracker)
		{
			transform2_param_t *xf = dynamic_cast<transform2_param_t*>( &param( "xf"));
			xf->apply_track( composition()->start_frame(), composition()->end_frame(), tracker, mode, use);
			update_widgets();
		}
        */
	}
}

node_t *create_move2d_node() { return new move2d_node_t();}

const node_metaclass_t *move2d_node_t::metaclass() const { return &move2d_node_metaclass();}

const node_metaclass_t& move2d_node_t::move2d_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.move2d";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Transform";
        info.menu_item = "Move2D";
        info.create = &create_move2d_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( move2d_node_t::move2d_node_metaclass());

} // namespace
} // namespace
