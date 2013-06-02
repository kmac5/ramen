// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UNDO_STACK_HPP
#define RAMEN_UNDO_STACK_HPP

#include<ramen/undo/stack_fwd.hpp>

#include<vector>
#include<stack>

#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_deque.hpp>

#include<ramen/assert.hpp>

#include<ramen/undo/command.hpp>

namespace ramen
{
namespace undo
{

/*!
\ingroup undo
\brief An undo stack.
*/
class stack_t : boost::noncopyable
{
public:

    /// Constructor.
    stack_t();

    /// Destructor.
    ~stack_t();

    /// Clears all redo commands.
    void clear_redo();

    /// Clears all commands.
    void clear_all();

    /// Pushes an undo command.
    template<class T>
    void push_back( std::auto_ptr<T> c)
    {
		RAMEN_ASSERT( dynamic_cast<command_t*>( c.get()) != 0); // I think this is not needed...
		undo_stack_.push_back( c.release());

		if( undo_stack_.size() > 50)
			undo_stack_.pop_front();
    }

    /// Erases the last undo command.
    void erase_last_command();

    /// Returns the last undo command.
    command_t& last_undo_command() { return undo_stack_.back();}

    /// Returns the last redo command.
    command_t& last_redo_command() { return redo_stack_.back();}

    /// Undoes the last command.
    void undo();

    /// Redoes the last command.
    void redo();

    /// Returns true if the undo stack is empty.
    bool undo_empty() const { return undo_stack_.empty();}

    /// Returns true if the redo stack is empty.
    bool redo_empty() const { return redo_stack_.empty();}
	
private:

    boost::ptr_deque<command_t> undo_stack_;
    boost::ptr_deque<command_t> redo_stack_;
};

} // namespace
} // namespace

#endif
