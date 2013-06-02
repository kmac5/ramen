// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/edge.hpp>

#include<ramen/assert.hpp>

namespace ramen
{

edge_t::edge_t() : src( 0), dst( 0), port( -1) {}

edge_t::edge_t( node_t *s, node_t *d, int p) : src( s), dst( d), port( p)
{
    RAMEN_ASSERT( s);
    RAMEN_ASSERT( d);
}

edge_t::edge_t( node_t *s, const adobe::name_t& splug, node_t *d, const adobe::name_t& dplug) : src( s), src_plug( splug), dst( d), dst_plug( dplug)
{
    RAMEN_ASSERT( s);
    RAMEN_ASSERT( d);
}

bool edge_t::operator==( const edge_t& other) const
{
    return src == other.src && dst == other.dst && port == other.port;
}


} // namespace

