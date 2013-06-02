// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_DRAG_KEYS_COMMAND_HPP
#define	RAMEN_UI_ANIM_DRAG_KEYS_COMMAND_HPP

#include<ramen/ui/anim/anim_editor_command.hpp>

#include<map>

#include<OpenEXR/ImathVec.h>

namespace ramen
{
namespace undo
{

class drag_keys_command_t : public anim_editor_command_t
{
public:

    drag_keys_command_t( node_t *node, const boost::shared_ptr<ui::track_model_t>& model);

	void start_drag( const Imath::V2f& offset, bool snap_frames);
	void drag_curve( anim::track_t *t);
	void end_drag();
	
private:
	
	Imath::V2f offset_;
	bool snap_frames_;
};

} // namespace
} // namespace

#endif
