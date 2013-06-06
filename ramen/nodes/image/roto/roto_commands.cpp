// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/roto/roto_commands.hpp>

#include<boost/foreach.hpp>

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>

#include<ramen/nodes/image/roto/roto_node.hpp>
#include<ramen/nodes/image/roto/shape.hpp>

#include<ramen/app/document.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>

namespace ramen
{
namespace undo
{

add_roto_command_t::add_roto_command_t( image::roto_node_t& node, std::auto_ptr<roto::shape_t> shape) : command_t( "Add Shape"), node_( node)
{
	RAMEN_ASSERT( shape.get());

    shape_ = shape.get();
	storage_ = shape;
}

void add_roto_command_t::undo()
{
	node_.deselect_all();
	node_.selection_changed();
	storage_ = node_.release_shape( shape_);
	app().ui()->anim_editor().recreate_tracks( &node_);
	node_.notify();
    undo::command_t::undo();
}

void add_roto_command_t::redo()
{
	node_.deselect_all();
	storage_->select( true);
	node_.add_shape( storage_);
	app().ui()->anim_editor().recreate_tracks( &node_);
	node_.selection_changed();
	node_.notify();
    undo::command_t::redo();
}

delete_roto_command_t::delete_roto_command_t( image::roto_node_t& node, roto::shape_t *shape) : command_t( "Delete Shape"), node_( node)
{
	shape_ = shape;
	parent_ = shape->parent();
	shape_->select( false);
	
	BOOST_FOREACH( roto::shape_t& s, shape_->children())
		children_.push_back( &s);
}

void delete_roto_command_t::undo()
{
	node_.deselect_all();
	node_.selection_changed();
	node_.add_shape( storage_);
	shape_->set_parent( parent_);

	for( int i = 0; i < children_.size(); ++i)
		children_[i]->set_parent( shape_);
	
	shape_->update_xforms();
	
	app().ui()->anim_editor().recreate_tracks( &node_);
	node_.deselect_all();
	node_.notify();
    undo::command_t::undo();	
}

void delete_roto_command_t::redo()
{
	node_.deselect_all();
	node_.selection_changed();
	storage_ = node_.release_shape( shape_);
	app().ui()->anim_editor().recreate_tracks( &node_);
	node_.notify();
    undo::command_t::redo();	
}

set_roto_parent_command_t::set_roto_parent_command_t( image::roto_node_t& node, roto::shape_t *shape, 
													  roto::shape_t *new_parent) : command_t( "Set Parent"), node_( node)
{
	shape_ = shape;
	new_parent_ = new_parent;
	old_parent_ = shape->parent();
}

void set_roto_parent_command_t::undo()
{
	shape_->set_parent( old_parent_);
	node_.selection_changed();
	node_.notify();
    undo::command_t::undo();
}

void set_roto_parent_command_t::redo()
{
	shape_->set_parent( new_parent_);
	node_.selection_changed();
	node_.notify();
    undo::command_t::redo();
}

modify_shape_command_t::modify_shape_command_t( image::roto_node_t& node, roto::shape_t *shape) : command_t( "Edit Shape"), node_( node)
{
    set_done( true);
	
	shape_ = shape;
	
	old_pts_ = shape_->triples();
	old_curve_ = shape_->anim_curve();
}

void modify_shape_command_t::undo()
{
	swap_shape_state();
	node_.notify();
    undo::command_t::undo();
}

void modify_shape_command_t::redo()
{
	swap_shape_state();
	node_.notify();
    undo::command_t::redo();
}

void modify_shape_command_t::swap_shape_state()
{
	shape_->triples().swap( old_pts_);
	shape_->anim_curve().swap( old_curve_);
	shape_->evaluate_shape_curve( app().document().composition().frame());
}

order_shape_command_t::order_shape_command_t( image::roto_node_t& node, roto::shape_t *shape, bool up) : command_t( "Change Shape Order"), node_( node)
{
	shape_ = shape;
	up_ = up;
}

void order_shape_command_t::undo()
{
	if( up_)
		node_.scene().move_shape_order_down( shape_);
	else
		node_.scene().move_shape_order_up( shape_);

	node_.selection_changed();
	node_.notify();
    undo::command_t::undo();
}

void order_shape_command_t::redo()
{
	if( up_)
		node_.scene().move_shape_order_up( shape_);
	else
		node_.scene().move_shape_order_down( shape_);

	node_.selection_changed();
	node_.notify();
    undo::command_t::redo();
}

} // namespace
} // namespace
