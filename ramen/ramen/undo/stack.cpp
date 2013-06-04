// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/undo/stack.hpp>

namespace ramen
{
namespace undo
{

stack_t::stack_t() {}
stack_t::~stack_t() {}

void stack_t::clear_redo()
{
    redo_stack_.clear();
}

void stack_t::clear_all()
{
    clear_redo();
    undo_stack_.clear();
}

void stack_t::erase_last_command() { undo_stack_.pop_back();}

void stack_t::undo()
{
    RAMEN_ASSERT( !undo_stack_.empty());
	
    undo_stack_.back().undo();
    boost::ptr_deque<command_t>::auto_type c( undo_stack_.pop_back());
    redo_stack_.push_back( c.release());
}

void stack_t::redo()
{
    RAMEN_ASSERT( !redo_stack_.empty());

    redo_stack_.back().redo();
    boost::ptr_deque<command_t>::auto_type c( redo_stack_.pop_back());
    undo_stack_.push_back( c.release());
}

} // namespace
} // namespace
