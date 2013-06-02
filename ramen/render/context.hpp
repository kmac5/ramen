// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_RENDER_CONTEXT_HPP
#define RAMEN_RENDER_CONTEXT_HPP

#include<ramen/config.hpp>

#include<ramen/image/format.hpp>

#include<boost/function.hpp>

#include<ramen/app/composition_fwd.hpp>
#include<ramen/nodes/node_fwd.hpp>

#include<ramen/util/error_log.hpp>

namespace ramen
{
namespace render
{

enum render_mode
{
    interface_render = 1,
    flipbook_render,
    process_render,
	analisys_render
};

struct RAMEN_API context_t
{
    context_t();

	bool render_cancelled() const;
	
	// data
	
    composition_t *composition;

    render_mode mode;

    node_t *result_node;

    double frame;
    int subsample;

    image::format_t default_format;

	int proxy_level;
	
    int motion_blur_extra_samples;
    float motion_blur_shutter_factor;

    boost::function<bool ()> cancel;
    util::error_log_t *error_log;
	
private:

	static bool default_cancel();	
};

} // namespace
} // namespace

#endif
