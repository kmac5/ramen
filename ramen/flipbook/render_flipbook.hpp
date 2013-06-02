// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_FLIPBOOK_HPP
#define RAMEN_RENDER_FLIPBOOK_HPP

#include<string>

#include<ramen/nodes/node_fwd.hpp>

#include<ramen/flipbook/flipbook.hpp>

namespace ramen
{
namespace flipbook
{

bool render_flipbook( flipbook::flipbook_t *flip, image_node_t *node,
						int start, int end, int proxy_level,
						int subsample, int mb_extra_samples, float mb_shutter_factor);

} // namespace
} // namespace

#endif
