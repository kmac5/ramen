// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UNDO_COMMAND_HPP
#define RAMEN_UNDO_COMMAND_HPP

#include<string>

#include<boost/function.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

namespace ramen
{
namespace undo
{

/*!
\ingroup undo
\brief Base class for actions that can be undone and redone.
*/
class command_t
{
public:

    /// Constructor.
	command_t();

    /*!
     Constructor.
     \param name Command name.
     */
    explicit command_t( const std::string& name);

    /// Destructor.
    virtual ~command_t();

    /// Returns this command name.
    const std::string& name() const;

    /// Sets this command name.
    void set_name( const std::string& name) { name_ = name;}

    /// Returns true if this command is already executed.
    bool done() const { return done_;}

    /// Sets if this command is executed.
    void set_done( bool b);

    /// For composite commands, returns true if this command is empty.
	virtual bool empty() const { return false;}
	
    /// Undoes this command.
    virtual void undo();

    /// Redoes this command.
    virtual void redo();

protected:

    std::string name_;
    bool was_dirty_;
    bool done_;
};

/*!
\ingroup undo
\brief Generic command that uses boost::functions for undo and redo.
*/
class generic_command_t : public command_t
{
public:

    typedef boost::function<void()> function_type;

    /*!
	 Constructor
     \param name This command name, for undo / redo menus.
	 \param undo_fun Function to call for undo.
	 \param redo_fun Function to call for redo.
	 */
    generic_command_t( const std::string& name, const function_type& undo_fun, const function_type& redo_fun);

    /// Undoes this command.
    virtual void undo();

    /// Redoes this command.
    virtual void redo();

private:

    function_type undo_;
    function_type redo_;
};

class composite_command_t : public command_t
{
public:

    /// Constructor.
    explicit composite_command_t( const std::string& name);

    /// Undoes this command.
    virtual void undo();

    /// Redoes this command.
    virtual void redo();

    /// Returns true if this command is empty.
    virtual bool empty() const { return commands_.empty();}

    /// Adds a child command to this command.
    void push_back( std::auto_ptr<command_t> c);

protected:

    boost::ptr_vector<command_t> commands_;
};

} // namespace
} // namespace

#endif
