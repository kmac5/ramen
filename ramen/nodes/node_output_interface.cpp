// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/node_output_interface.hpp>

namespace ramen
{

void node_output_interface::begin_output( float start_frame, float end_frame) {}
void node_output_interface::end_output( bool success) {}

} // ramen
