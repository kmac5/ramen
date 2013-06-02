// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/flipbook/render_flipbook.hpp>

#include<boost/signals2/connection.hpp>

#include<OpenEXR/Iex.h>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>
#include<ramen/app/preferences.hpp>

#include<ramen/nodes/image_node.hpp>

#include<ramen/render/context_guard.hpp>
#include<ramen/render/image_node_renderer.hpp>

namespace ramen
{
namespace flipbook
{
namespace
{

bool render_cancelled;

void cancel_render() { render_cancelled = true;}

} // unnamed

bool render_flipbook( flipbook::flipbook_t *flip, image_node_t *node,
						int start, int end, int proxy_level,
						int subsample, int mb_extra_samples, float mb_shutter_factor)
{
    render_cancelled = false;
    boost::signals2::connection connection = flip->cancelled.connect( &cancel_render);

    render::context_t cur_context = app().document().composition().current_context();
	render::context_guard_t guard( cur_context);	
	
    render::context_t new_context = cur_context;
    new_context.mode = render::flipbook_render;
    new_context.result_node = node;
	new_context.proxy_level = proxy_level;
    new_context.subsample = subsample;
    new_context.motion_blur_extra_samples = mb_extra_samples;
    new_context.motion_blur_shutter_factor = mb_shutter_factor;

	flip->set_frame_range( start, end);
	flip->begin_progress();
	
    // recalc the format, in case this is our first render
    node->recursive_calc_format( new_context);
	flip->set_format( node->format(), node->aspect_ratio(), subsample);
	
    for( int i = start; i <= end; ++i)
    {
        try
        {
            new_context.frame = i;

			render::image_node_renderer_t renderer( new_context);
            renderer.render();

            if( render_cancelled)
            {
                connection.disconnect();
                return false;
            }
			
			flip->add_frame( i, node->image());
        }
		catch( std::bad_alloc& e)
		{
			connection.disconnect();
			flip->end_progress();			
            return !flip->empty();
		}
		catch( std::exception& e)
		{
			app().error( std::string( "exception thrown during render flipbook. what = ") + e.what());
			connection.disconnect();
			flip->end_progress();			
            return !flip->empty();
		}
        catch( ...)
        {
			app().error( std::string( "unknown exception thrown during render flipbook"));
			connection.disconnect();
			flip->end_progress();			
            return !flip->empty();
        }
    }

    connection.disconnect();
	flip->end_progress();			
    return true;
}

} // namespace
} // namespace
