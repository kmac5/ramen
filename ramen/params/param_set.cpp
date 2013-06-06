// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/param_set.hpp>

#include<cstdlib>
#include<cstdio>

#include<stdexcept>
#include<algorithm>
#include<sstream>

#include<boost/bind.hpp>
#include<boost/foreach.hpp>

#include<boost/ptr_container/ptr_vector.hpp>
#include<boost/ptr_container/ptr_map.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/params/parameterised.hpp>
#include<ramen/params/param.hpp>
#include<ramen/params/composite_param.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/viewer/viewer.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>
#include<ramen/serialization/yaml_node.hpp>

#include<iostream>

namespace ramen
{

class param_set_command_t : public undo::command_t
{
public:

    param_set_command_t( const std::string& name) : command_t( name) {}

    virtual bool empty() const
    {
        if( !commands_.empty())
            return false;

        for( boost::ptr_vector<undo::command_t>::const_iterator it( more_commands_.begin()); it != more_commands_.end(); ++it)
        {
            if( !it->empty())
                return false;
        }

        return true;
    }

    bool has_command_for_param( param_t *p) { return commands_.find( p) != commands_.end();}

    void add_command( param_t *p, std::auto_ptr<undo::command_t> c) { commands_.insert( p, c);}
    void add_command( std::auto_ptr<undo::command_t> c) { more_commands_.push_back( c);}

    virtual void undo()
    {
        param_set_t *pset = commands_.begin()->first->param_set();

        for( boost::ptr_map<param_t*, undo::command_t>::iterator it( commands_.begin()); it != commands_.end(); ++it)
            it->second->undo();

        for( boost::ptr_vector<undo::command_t>::iterator it( more_commands_.begin()); it != more_commands_.end(); ++it)
            it->undo();

        pset->notify_parent();
        undo::command_t::undo();
    }

    virtual void redo()
    {
        param_set_t *pset = commands_.begin()->first->param_set();

        for( boost::ptr_map<param_t*, undo::command_t>::iterator it( commands_.begin()); it != commands_.end(); ++it)
            it->second->redo();

        for( boost::ptr_vector<undo::command_t>::iterator it( more_commands_.begin()); it != more_commands_.end(); ++it)
            it->redo();

        pset->notify_parent();
        undo::command_t::redo();
    }

private:

    boost::ptr_map<param_t*, undo::command_t> commands_;
    boost::ptr_vector<undo::command_t> more_commands_;
};

param_set_t::param_set_t( parameterised_t *p) : parent_( p) {}

param_set_t::param_set_t( const param_set_t& other) : params_( other.params_)
{
    parent_ = 0;
    adobe::for_each( params_, boost::bind( &param_t::set_param_set, _1, this));
}

param_set_t::~param_set_t()
{
    // empty destructor to allow auto_prt to use an incomplete type.
    // Do not remove.
}

void param_set_t::clear()
{
    command_.reset();
    params_.clear();
}

void param_set_t::do_add_param( param_t *p)
{
    p->set_param_set( this);
    params_.push_back( p);
}

const param_t& param_set_t::find( const std::string& id) const
{
    param_set_t& self = const_cast<param_set_t&>( *this);
    return self.find( id);
}

param_t& param_set_t::find( const std::string& id)
{
    RAMEN_ASSERT( !id.empty());

    BOOST_FOREACH( param_t& p, params())
    {
        if( p.id() == id)
            return p;

        if( composite_param_t *cp = dynamic_cast<composite_param_t*>( &p))
        {
            param_t *q = cp->find( id);

            if( q)
                return *q;
        }
    }

    throw std::runtime_error( std::string( "Param not found: ").append( id));
}

void param_set_t::notify_parent()
{
    if( parent())
        parent()->param_edit_finished();
}

void param_set_t::begin_edit()
{
    command_.reset( new param_set_command_t( "Params Changed"));
}

void param_set_t::end_edit( bool notify)
{
    if( command_.get() && !is_command_empty())
        app().document().undo_stack().push_back( command_);
    else
        command_.reset();

    if( notify)
        notify_parent();

    app().ui()->update();
    app().ui()->update_anim_editors();
}

bool param_set_t::editing() const { return command_.get();}

const param_set_command_t *param_set_t::command() const { return command_.get();}
param_set_command_t	*param_set_t::command()				{ return command_.get();}

bool param_set_t::is_command_empty() const
{
    if( !command_.get())
        return true;

    return command_->empty();
}

void param_set_t::add_command( param_t *p)
{
    if( editing())
    {
        if( !command_->has_command_for_param( p))
        {
            std::auto_ptr<undo::command_t> c( p->create_command());

            if( c.get())
                command_->add_command( p, c);
        }
   }
}

bool param_set_t::autokey() const
{
    if( parent())
        return parent()->autokey();
    else
        return false;
}

void param_set_t::add_to_hash( hash::generator_t& hash_gen) const
{
    BOOST_FOREACH( const param_t& p, params())
    {
        if( p.include_in_hash())
            p.add_to_hash( hash_gen);
    }
}

void param_set_t::for_each_param( const boost::function<void ( param_t*)>& f)
{
    adobe::for_each( params_, boost::bind( &param_t::apply_function, _1, f));
}

// serialization

void param_set_t::read( const serialization::yaml_node_t& node)
{
    for( int i = 0; i < node.size(); ++i)
        read_param( node[i]);
}

void param_set_t::read_param( const serialization::yaml_node_t& node)
{
    std::string id;
    node.get_value( "id", id);

    try
    {
        param_t& p( find( id));
        p.read( node);
    }
    catch( YAML::Exception& e)
    {
        node.error_stream() << "Yaml exception: " << e.what() << " in node " << parent()->name() << "\n";
    }
    catch( std::runtime_error& e)
    {
        node.error_stream() << "Unknown param " << id << " in node " << parent()->name() << "\n";
    }
}

void param_set_t::write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "params" << YAML::Value;
        out.begin_seq();
            BOOST_FOREACH( const param_t& p, params()) { p.write( out);}
        out.end_seq();
}

} // namespace
