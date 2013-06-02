// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ui/render_composition.hpp>

#include<boost/foreach.hpp>
#include<boost/thread.hpp>
#include<boost/bind.hpp>

#include<QProgressDialog>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>
#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/render/context_guard.hpp>
#include<ramen/render/render_sequence.hpp>

namespace ramen
{
namespace ui
{
namespace
{

struct render_callback
{
	render_callback( QProgressDialog *p) : progress_( p) {}
	
	bool operator()( int x)
	{
		progress_->setValue( x);
		
		for( int i = 0; i < 10; ++i)
			app().ui()->process_events();
		
		return progress_->wasCanceled();
	}
	
private:
	
	QProgressDialog *progress_;
};

struct cancel_callback
{
	cancel_callback( QProgressDialog *p) : progress_( p) {}
	
	bool operator()()
	{
		for( int i = 0; i < 10; ++i)
			app().ui()->process_events();
			
		return progress_->wasCanceled();
	}
	
private:
	
	QProgressDialog *progress_;
};

} // unnamed

void render_composition( composition_t& comp, int start, int end, int proxy_level,
						 int subsample, int mb_extra_samples, float mb_shutter_factor, bool selected_only)
{
	int num_frames = render::total_frames_to_render( comp, start, end, selected_only);

	if( num_frames == 0)
		return;

	int rendered_frames = 0;
    bool stop = false;
	bool success = false;
	
	QProgressDialog progress( "Rendering", "Cancel", 0, num_frames, (QWidget *) app().ui()->main_window());
	progress.setWindowModality( Qt::ApplicationModal);
	progress.setWindowTitle( "Rendering");
	progress.show();
	
	render_callback progress_cb( &progress);
	cancel_callback cancel_cb( &progress);
	
	if( progress_cb( rendered_frames))
		return;

    render::context_t cur_context = comp.current_context();
	render::context_guard_t guard( cur_context);
	
	// call begin output for each output
	render::for_each_output( comp, selected_only, boost::bind( &node_output_interface::begin_output, _1, start, end));
		
    render::context_t new_context = cur_context;
    new_context.mode = render::process_render;
	new_context.proxy_level = proxy_level;
    new_context.subsample = subsample;
    new_context.motion_blur_extra_samples = mb_extra_samples;
    new_context.motion_blur_shutter_factor = mb_shutter_factor;
	new_context.cancel = cancel_cb;
		
	for( int i = start; i <= end; ++i, ++rendered_frames)
	{
        new_context.frame = i;

        BOOST_FOREACH( node_t& n, comp.nodes())
        {
            if( node_output_interface *out = dynamic_cast<node_output_interface*>( &n))
            {
                if( ( selected_only && !n.selected()) || n.ignored())
                    continue;
				
				new_context.result_node = &n;

				try
				{
					out->process_and_write( new_context);
				}
				catch( std::exception& e)
				{
                    app().ui()->error( std::string( "exception thrown during render sequence. what = ") + e.what());
				}
			}
			
			if( progress_cb( rendered_frames))
				goto cleanup;
		}
	}
	
	if( !progress.wasCanceled())
		success = true;
	else
		progress.setValue( num_frames);

cleanup:
	
	render::for_each_output( comp, selected_only, boost::bind( &node_output_interface::end_output, _1, success));	
}

} // namespace
} // namespace
