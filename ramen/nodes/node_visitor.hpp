// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_VISITOR_HPP
#define	RAMEN_NODE_VISITOR_HPP

#include<ramen/config.hpp>

#include<ramen/nodes/node_fwd.hpp>

namespace ramen
{

class node_visitor
{
public:

    virtual ~node_visitor();

    virtual void visit( node_t *n) = 0;
    virtual void visit( image_node_t *n) = 0;
};

class generic_node_visitor : public node_visitor
{
public:

    virtual ~generic_node_visitor();

    virtual void visit( node_t *n) = 0;
    virtual void visit( image_node_t *n);
};

} // ramen

#endif
