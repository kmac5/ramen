// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_NODE_OUTPUT_INTERFACE_HPP
#define	RAMEN_NODE_OUTPUT_INTERFACE_HPP

#include<ramen/render/context.hpp>

namespace ramen
{

class node_output_interface
{
public:

    virtual int priority() const { return 100;}

    virtual void begin_output( float start_frame, float end_frame);
    virtual void process_and_write( const render::context_t& context) = 0;
    virtual void end_output( bool success);
};

} // ramen

#endif
