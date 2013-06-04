// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/anim/drag_keys_command.hpp>

#include<ramen/ui/anim/edit_keys_visitors.hpp>

#include<ramen/anim/track.hpp>

namespace ramen
{
namespace undo
{

drag_keys_command_t::drag_keys_command_t( node_t *node, const boost::shared_ptr<ui::track_model_t>& model) : anim_editor_command_t( node, model)
{
}

void drag_keys_command_t::start_drag( const Imath::V2f& offset, bool snap_frames)
{
	offset_ = offset;
	snap_frames_ = snap_frames;
}

void drag_keys_command_t::drag_curve( anim::track_t *t)
{	
	ui::can_drag_keys_visitor v0( offset_);
	boost::apply_visitor( v0, t->curve().get());

	// if keys can be dragged
	if( v0.result)
	{
		add_track( t);
	
		iterator it = curves_.find( t);
		RAMEN_ASSERT( it != curves_.end());
		
		// restore the original curve
		anim::copy( it->second, t->curve().get());
	
		// and drag it.
		ui::drag_keys_visitor v1( offset_, snap_frames_);
		boost::apply_visitor( v1, t->curve().get());
	}
}

void drag_keys_command_t::end_drag()
{
	call_notify_for_tracks();
	notify_nodes();
}

} // namespace
} // namespace
