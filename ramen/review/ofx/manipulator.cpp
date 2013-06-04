// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ofx/manipulator.hpp>

#include<ramen/assert.hpp>

#include<ramen/ofx/image_effect.hpp>
#include<ramen/ofx/ofx_node.hpp>
#include<ramen/ofx/util.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/viewer/image_view/image_viewer_strategy.hpp>

#ifndef NDEBUG
	#include<glog/logging.h>
#endif

namespace ramen
{
namespace ofx
{

bool manipulator_t::mouse_down_ = false;
	
manipulator_t::manipulator_t( image::ofx_node_t *node, image_effect_t *effect) : ramen::manipulator_t(),
																				OFX::Host::Interact::Instance( effect->getOverlayDescriptor( 8, true), 
																											   (void*) effect)
{
    RAMEN_ASSERT( node && "null ofx_node in manipulator_t");
    RAMEN_ASSERT( effect && "null image effect in manipulator_t");

    OfxStatus stat = createInstanceAction();

    if( stat != kOfxStatOK && stat != kOfxStatReplyDefault)
		throw std::runtime_error( "Error creating OFX overlay interact");
}

const image::ofx_node_t *manipulator_t::ofx_node() const
{
    const image::ofx_node_t *node = dynamic_cast<const image::ofx_node_t*>( parent());
    RAMEN_ASSERT( node);
    return node;
}

image::ofx_node_t *manipulator_t::ofx_node()
{
    image::ofx_node_t *node = dynamic_cast<image::ofx_node_t*>( parent());
    RAMEN_ASSERT( node);
    return node;
}

void manipulator_t::do_draw_overlay( const ui::paint_event_t& event) const
{
	ui::viewer::image_viewer_strategy_t *iview = const_cast<ui::viewer::image_viewer_strategy_t*>( 
												dynamic_cast<const ui::viewer::image_viewer_strategy_t*>( event.view));
	RAMEN_ASSERT( iview);
	
	iview->save_projection();
    iview->set_ofx_projection( ofx_node()->format());

    OfxPointD render_scale;
    render_scale.x = 1;
    render_scale.y = 1;

    manipulator_t *this_ = const_cast<manipulator_t*>( this);
    this_->drawAction( ofx_node()->composition()->frame(), render_scale);

    iview->restore_projection();
}

bool manipulator_t::do_key_press_event( const ui::key_press_event_t& event) { return false;}
		
void manipulator_t::do_key_release_event( const ui::key_release_event_t& event) {}

void manipulator_t::do_mouse_enter_event( const ui::mouse_enter_event_t& event)
{
    image::ofx_node_t *node = ofx_node();
    RAMEN_ASSERT( node);
	
    OfxPointD render_scale;
    render_scale.x = 1.0;
    render_scale.y = 1.0;
    gainFocusAction( node->composition()->frame(), render_scale);	
}

void manipulator_t::do_mouse_leave_event( const ui::mouse_leave_event_t& event)
{
    image::ofx_node_t *node = ofx_node();
    RAMEN_ASSERT( node);

    OfxPointD render_scale;
    render_scale.x = 1.0;
    render_scale.y = 1.0;
    loseFocusAction( node->composition()->frame(), render_scale);
}

bool manipulator_t::do_mouse_press_event( const ui::mouse_press_event_t& event)
{
    image::ofx_node_t *node = ofx_node();
    RAMEN_ASSERT( node);

	mouse_down_ = true;
	node->param_set().begin_edit();
    penDownAction( node->composition()->frame(), get_render_scale( node, event), 
				   get_pen_pos( node, event), get_mouse_pos( node, event), 1);
    return true;
}

void manipulator_t::do_mouse_move_event( const ui::mouse_move_event_t& event)
{
    image::ofx_node_t *node = ofx_node();
    RAMEN_ASSERT( node);

    penMotionAction( node->composition()->frame(), get_render_scale( node, event), 
					 get_pen_pos( node, event), get_mouse_pos( node, event), 0);
}

void manipulator_t::do_mouse_drag_event( const ui::mouse_drag_event_t& event)
{
    image::ofx_node_t *node = ofx_node();
    RAMEN_ASSERT( node);
	
    penMotionAction( node->composition()->frame(), get_render_scale( node, event), 
					 get_pen_pos( node, event), get_mouse_pos( node, event), 1);
}

void manipulator_t::do_mouse_release_event( const ui::mouse_release_event_t& event)
{
    image::ofx_node_t *node = ofx_node();
    RAMEN_ASSERT( node);

    penUpAction( node->composition()->frame(), get_render_scale( node, event), 
				 get_pen_pos( node, event), get_mouse_pos( node, event), 0);	

	mouse_down_ = false;
	
	bool is_dirty = !node->param_set().is_command_empty();
	node->param_set().end_edit( false);
	
	if( is_dirty)
		node->notify();
}

bool manipulator_t::mouse_down() { return mouse_down_;}

OfxPointD manipulator_t::get_render_scale( const image::ofx_node_t *node, const ui::mouse_event_t& event) const
{
    OfxPointD render_scale;
    render_scale.x = 1.0 / event.subsample;
    render_scale.y = 1.0 / event.subsample;
	return render_scale;
}

OfxPointI manipulator_t::get_mouse_pos( const image::ofx_node_t *node, const ui::mouse_event_t& event) const
{
    OfxPointI mouse_pos;
    mouse_pos.x = event.pos.x;
    mouse_pos.y = event.pos.y;
	return mouse_pos;
}

OfxPointD manipulator_t::get_pen_pos( const image::ofx_node_t *node, const ui::mouse_event_t& event) const
{
	RAMEN_ASSERT( node);

    OfxPointD pen_pos;
    Imath::V2f world( node->vertical_flip( event.wpos));
    //world /= event.subsample;
    pen_pos.x = world.x;
    pen_pos.y = world.y;
	return pen_pos;
}

void manipulator_t::getViewportSize(double &width, double &height) const
{
	RAMEN_ASSERT( 0);
    width  = ui::viewer_t::Instance().width();
    height = ui::viewer_t::Instance().height();
}

void manipulator_t::getPixelScale(double& xScale, double& yScale) const
{
	ui::viewer::image_viewer_strategy_t *iview = dynamic_cast<ui::viewer::image_viewer_strategy_t*>( &ui::viewer_t::Instance().current_viewer());
	RAMEN_ASSERT( iview);
	
    xScale = yScale = 1.0 / iview->pixel_scale();
}

void manipulator_t::getBackgroundColour(double &r, double &g, double &b) const
{
    Imath::Color3c col( ui::palette_t::Instance().color("background"));
    r = col.x / 255.0;
    r = col.y / 255.0;
    r = col.z / 255.0;
}

OfxStatus manipulator_t::swapBuffers()
{
    ui::viewer_t::Instance().swap_buffers();
    return kOfxStatOK;
}

OfxStatus manipulator_t::redraw()
{
    ui::viewer_t::Instance().update();
    return kOfxStatOK;
}

} // namespace
} // namespace
