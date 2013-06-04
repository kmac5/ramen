// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAM_PARAM_SET_HPP
#define	RAMEN_PARAM_PARAM_SET_HPP

#include<ramen/params/param_set_fwd.hpp>

#include<ramen/python/python.hpp>

#include<map>
#include<memory>
#include<string>

#include<boost/signals2/signal.hpp>

#include<ramen/params/param.hpp>
#include<ramen/params/parameterised_fwd.hpp>

#include<ramen/python/access_fwd.hpp>

namespace ramen
{

class composite_param_t;

class param_set_command_t;

/*!
\ingroup params
\brief A container for params, with some extra functionality
*/
class RAMEN_API param_set_t
{
public:

    param_set_t( parameterised_t *p = 0);
    param_set_t( const param_set_t& other);

    ~param_set_t();

    const parameterised_t *parent() const   { return parent_;}
    parameterised_t *parent()				{ return parent_;}
    void set_parent( parameterised_t *p)    { parent_ = p;}

    void clear();
    bool empty() const { return params_.empty();}

    // iterator and ranges

    const boost::ptr_vector<param_t>& params() const    { return params_;}
    boost::ptr_vector<param_t>& params()                { return params_;}

    typedef boost::ptr_vector<param_t>::const_iterator const_iterator;
    typedef boost::ptr_vector<param_t>::iterator iterator;

    const_iterator begin() const    { return params_.begin();}
    const_iterator end() const	    { return params_.end();}

    iterator begin()	{ return params_.begin();}
    iterator end()		{ return params_.end();}

    template<class T>
    void add_param( std::auto_ptr<T> p)
    {
        do_add_param( p.release());
    }

    const param_t& find( const std::string& id) const;
    param_t& find( const std::string& id);

    boost::signals2::signal<void ( param_t*, param_t::change_reason)> param_changed;
    void notify_parent();

    void begin_edit();
    void end_edit( bool notify = true);

    bool editing() const;

    const param_set_command_t *command() const;
    param_set_command_t	*command();

    bool is_command_empty() const;

    void add_command( param_t *p);

    bool autokey() const;

    void add_to_hash( hash::generator_t& hash_gen) const;

    // util
    void for_each_param( const boost::function<void ( param_t*)>& f);

    // serialization
    void read( const serialization::yaml_node_t& node);
    void write( serialization::yaml_oarchive_t& out) const;

    void read_param( const serialization::yaml_node_t& node);

private:

    friend class python::access;

    void do_add_param( param_t *p);

    parameterised_t *parent_;
    boost::ptr_vector<param_t> params_;
    std::auto_ptr<param_set_command_t> command_;
};

} // namespace

#endif
