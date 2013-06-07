// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_COMMANDS_HPP
#define	RAMEN_ROTO_COMMANDS_HPP

#include<ramen/undo/command.hpp>

#include<vector>

#include<ramen/nodes/image/roto/roto_node_fwd.hpp>
#include<ramen/nodes/image/roto/shape.hpp>

namespace ramen
{
namespace undo
{

class add_roto_command_t : public command_t
{
public:

    add_roto_command_t( image::roto_node_t& node, std::auto_ptr<roto::shape_t> shape);

    virtual void undo();
    virtual void redo();

private:

    image::roto_node_t& node_;
    roto::shape_t *shape_;
    std::auto_ptr<roto::shape_t> storage_;
};

class delete_roto_command_t : public command_t
{
public:

    delete_roto_command_t( image::roto_node_t& node, roto::shape_t *shape);

	virtual void undo();
	virtual void redo();

private:

    image::roto_node_t& node_;
    roto::shape_t *shape_, *parent_;
    std::auto_ptr<roto::shape_t> storage_;
	std::vector<roto::shape_t*> children_;
};

class set_roto_parent_command_t : public command_t
{
public:
	
	set_roto_parent_command_t( image::roto_node_t& node, roto::shape_t *shape, roto::shape_t *new_parent);

    virtual void undo();
    virtual void redo();

private:

    image::roto_node_t& node_;
    roto::shape_t *shape_, *new_parent_, *old_parent_;
};

class modify_shape_command_t : public command_t
{
public:
	
	modify_shape_command_t( image::roto_node_t& node, roto::shape_t *shape);

    virtual void undo();
    virtual void redo();

private:

	void swap_shape_state();
	
    image::roto_node_t& node_;
    roto::shape_t *shape_;
	
	std::vector<roto::triple_t> old_pts_;
	anim::shape_curve2f_t old_curve_;
};

class order_shape_command_t : public command_t
{
public:
	
	order_shape_command_t( image::roto_node_t& node, roto::shape_t *shape, bool up);

    virtual void undo();
    virtual void redo();

private:
	
    image::roto_node_t& node_;
    roto::shape_t *shape_;
	bool up_;
};

} // undo
} // ramen

#endif
