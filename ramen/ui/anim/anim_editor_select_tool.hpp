// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_EDITOR_SELECT_TOOL_HPP
#define	RAMEN_UI_ANIM_EDITOR_SELECT_TOOL_HPP

#include<ramen/ui/anim/anim_editor_tool.hpp>

#include<memory>

#include<ramen/anim/track_fwd.hpp>
#include<ramen/anim/any_curve.hpp>

#include<ramen/ui/anim/drag_keys_command.hpp>

namespace ramen
{
namespace ui
{

class anim_editor_select_tool_t : public anim_editor_tool_t
{
public:

    anim_editor_select_tool_t();

    virtual void draw_overlay( anim_curves_view_t& view) const;

    virtual void key_press_event( anim_curves_view_t& view, QKeyEvent *event);
    virtual void key_release_event( anim_curves_view_t& view, QKeyEvent *event);

    virtual void mouse_press_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_move_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_drag_event( anim_curves_view_t& view, QMouseEvent *event);
    virtual void mouse_release_event( anim_curves_view_t& view, QMouseEvent *event);

private:

	void select_keyframe( anim::any_curve_ptr_t& c, int index);
	void toggle_select_keyframe( anim::any_curve_ptr_t& c, int index);
	bool keyframe_is_selected( anim::any_curve_ptr_t& c, int index) const;

	void create_drag_command();
	
    int push_x_, push_y_;
    int last_x_, last_y_;

    bool dragged_;
    bool drag_keyframes_mode_;
    bool box_pick_mode_;
    bool drag_tangents_mode_;
    bool insert_keyframe_;
	
	anim::track_t *track_;
    int key_index_;
    bool left_;
	bool break_tangents_;
	
	undo::drag_keys_command_t *drag_command_;
};

} // namespace
} // namespace

#endif
