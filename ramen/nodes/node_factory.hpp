// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_FACTORY_HPP
#define RAMEN_NODE_FACTORY_HPP

#include<utility>
#include<vector>
#include<memory>
#include<map>

#include<boost/noncopyable.hpp>

#include<ramen/nodes/node_metaclass.hpp>

namespace ramen
{

/*!
\ingroup nodes
\brief Singleton node factory.
*/
class node_factory_t : boost::noncopyable
{
public:

    static node_factory_t& instance();

    bool register_node( const node_metaclass_t& m);

    void sort_by_menu_item();

    // all metaclasses
    const std::vector<node_metaclass_t>& registered_nodes() const   { return metaclasses_;}
    std::vector<node_metaclass_t>& registered_nodes()               { return metaclasses_;}

    // latest versions
    typedef std::map<std::string, node_metaclass_t>::const_iterator const_iterator;
    typedef std::map<std::string, node_metaclass_t>::iterator       iterator;

    const_iterator latest_versions_begin() const    { return newest_node_infos_.begin();}
    const_iterator latest_versions_end() const	    { return newest_node_infos_.end();}

    iterator latest_versions_begin()    { return newest_node_infos_.begin();}
    iterator latest_versions_end()      { return newest_node_infos_.end();}

    bool is_latest_version( const std::string& id) const;

    // creation
    std::auto_ptr<node_t> create_by_id( const std::string& id, bool ui = false);
    std::auto_ptr<node_t> create_by_id_with_version( const std::string& id, const std::pair<int, int>& version);

private:

    node_factory_t();
    ~node_factory_t();

    std::vector<node_metaclass_t> metaclasses_;
    std::map<std::string, node_metaclass_t> newest_node_infos_;
};

} // ramen

#endif
