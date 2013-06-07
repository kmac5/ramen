// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TRACKER_MANIPULATOR_HPP
#define RAMEN_IMAGE_TRACKER_MANIPULATOR_HPP

#include<ramen/manipulators/manipulator.hpp>

#include<ramen/nodes/image/track/tracker_node_fwd.hpp>
#include<ramen/nodes/image/track/ncc_tracker_fwd.hpp>

namespace ramen
{
	
class tracker_manipulator_t : public manipulator_t
{
public:

    tracker_manipulator_t();

private:
	
	const image::tracker_node_t *tracker_node() const;
	image::tracker_node_t *tracker_node();
	
    virtual void do_draw_overlay( const ui::paint_event_t& event) const;

    virtual bool do_key_press_event( const ui::key_press_event_t& event);
    virtual void do_key_release_event( const ui::key_release_event_t& event);

    virtual bool do_mouse_press_event( const ui::mouse_press_event_t& event);
    virtual void do_mouse_drag_event( const ui::mouse_drag_event_t& event);
    virtual void do_mouse_release_event( const ui::mouse_release_event_t& event);
	
	void draw_tracker( const track::ncc_tracker_t *t, const ui::paint_event_t& event) const;
	void draw_tracker_path( const track::ncc_tracker_t *t, const ui::paint_event_t& event, const Imath::Color3c& color) const;
	
	bool pick_tracker( const track::ncc_tracker_t *t, const ui::mouse_event_t& event);
	
	enum drag_mode
	{
		no_drag,
		track_drag,
		track_key_drag,
		offset_drag,
		ref_drag,
		search_drag
	};			

	int picked_tracker_;
	drag_mode mode_;
	int picked_corner_;
	float frame_;
};

} // namespace

#endif
