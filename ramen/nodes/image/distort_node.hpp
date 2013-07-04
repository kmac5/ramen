// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_DISTORT_NODE_HPP
#define RAMEN_IMAGE_DISTORT_NODE_HPP

#include<ramen/nodes/image/base_warp_node.hpp>

namespace ramen
{
namespace image
{

class RAMEN_API distort_node_t : public base_warp_node_t
{
protected:

	distort_node_t();

    distort_node_t( const distort_node_t& other);
    void operator=( const distort_node_t&);
};

} // image
} // ramen

#endif
