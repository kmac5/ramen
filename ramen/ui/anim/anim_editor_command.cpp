// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/anim/anim_editor_command.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>
#include<adobe/algorithm/find.hpp>

#include<ramen/app/application.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/nodes/graph_algorithm.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/anim/anim_editor_toolbar.hpp>

namespace ramen
{
namespace undo
{

anim_editor_command_t::anim_editor_command_t( node_t *node, const boost::shared_ptr<ui::track_model_t>& model) : command_t( "Edit Curves")
{
	RAMEN_ASSERT( model.get());
	RAMEN_ASSERT( node);
	
	nodes_.insert( node);
	model_ = model;
	set_done( true);
}

bool anim_editor_command_t::empty() const { return curves_.empty();}

void anim_editor_command_t::add_track( anim::track_t *t)
{
	RAMEN_ASSERT( t != 0);
	RAMEN_ASSERT( t->is_leaf());

	// check if not already inserted
	if( curves_.find( t) == curves_.end())
	{
		anim::any_curve_t new_curve;
		anim::copy( t->curve().get(), new_curve);
		curves_[t] = new_curve;
	}
}

void anim_editor_command_t::call_notify_for_tracks()
{
	for( iterator it( curves_.begin()); it != curves_.end();++it)
		it->first->notify();
}

void anim_editor_command_t::swap_curves()
{
	for( iterator it( curves_.begin()); it != curves_.end();++it)
        anim::swap( const_cast<anim::any_curve_ptr_t&>( it->first->curve().get()), it->second);		
}

void anim_editor_command_t::notify_nodes()
{
	RAMEN_ASSERT( !nodes_.empty());

	if( empty())
		return; // nothing to do.
	
	// This is the right way to do it, but for now, we only have 1 active node.
	//breadth_first_multiple_outputs_search( nodes_, boost::bind( &node_t::do_notify, _1), true);
	(*nodes_.begin())->notify();
}

void anim_editor_command_t::undo()
{
    swap_curves();
	call_notify_for_tracks();
	notify_nodes();
	app().ui()->anim_editor().toolbar().selection_changed();
    command_t::undo();
}

void anim_editor_command_t::redo()
{
    swap_curves();
	call_notify_for_tracks();
	notify_nodes();
	app().ui()->anim_editor().toolbar().selection_changed();
    command_t::redo();
}

} // namespace
} // namespace
