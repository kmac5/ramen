// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image_node.hpp>

#include<boost/foreach.hpp>
#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>

#include<ramen/memory/manager.hpp>

#include<ramen/nodes/graph_algorithm.hpp>

#include<ramen/image/color_bars.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>

#include<ramen/ui/palette.hpp>

#include<iostream>

namespace ramen
{

image_node_t::image_node_t() : node_t() {}
image_node_t::image_node_t( const image_node_t& other) : node_t( other) {}

void image_node_t::cloned() { format_changed();}

void image_node_t::accept( node_visitor& v) { v.visit( this);}

void image_node_t::add_output_plug()
{
    node_t::add_output_plug( "output", ui::palette_t::instance().color( "out plug"), "output" );
}

void image_node_t::do_notify()
{
    // keep the format up to date
    Imath::Box2i old_format( full_format());
	float old_aspect = aspect_ratio();
	Imath::V2f old_proxy_scale = proxy_scale();

    render::context_t context = composition()->current_context();
    context.subsample = 1;
    calc_format( context);

    if( old_format != format() || old_aspect != aspect_ratio() || old_proxy_scale != proxy_scale())
        format_changed();

    node_t::do_notify();
}

bool image_node_t::accept_connection( node_t *src, int port) const
{
    // by default, we only accept image nodes
    return dynamic_cast<image_node_t*>( src) != 0;
}

void image_node_t::format_changed()
{
    adobe::for_each( param_set(), boost::bind( &param_t::format_changed, _1, format(), aspect_ratio(), proxy_scale()));
}

void image_node_t::set_format( const Imath::Box2i& d) { format_ = d;}

void image_node_t::calc_format( const render::context_t& context)
{
    is_valid_ = is_valid();
	is_identity_ = false;

	if( is_valid_)
		is_identity_ = is_identity();

	// init with invalid values, to catch the case 
	// when we forget to set them.
	aspect_ = 0.0f;
	proxy_scale_ =  Imath::V2f( 0, 0);

	if( is_valid_ && !is_identity_)
		do_calc_format( context);
	else
		image_node_t::do_calc_format( context);
	
	// save the full resolution format.
	full_format_ = format_;
	
	// make sure the values have been initialized
	RAMEN_ASSERT( aspect_ratio() != 0.0f);
	RAMEN_ASSERT( proxy_scale().x != 0.0f && proxy_scale().y != 0.0f);
}

void image_node_t::do_calc_format( const render::context_t& context)
{
    if( ( num_inputs() != 0) && input_as<image_node_t>())
    {
        image_node_t *in = input_as<image_node_t>();
        set_format( in->format());
		set_aspect_ratio( in->aspect_ratio());
		set_proxy_scale( in->proxy_scale());
    }
	else
	{
		// init with default values
		set_format( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( context.default_format.area().max.x - 1, 
																 context.default_format.area().max.y - 1)));
		set_aspect_ratio( context.default_format.aspect);
		set_proxy_scale( Imath::V2f( 1.0f, 1.0f));
	}
}

void image_node_t::set_aspect_ratio( float a)
{ 
	RAMEN_ASSERT( a > 0);
	aspect_ = a;
}

void image_node_t::set_proxy_scale( const Imath::V2f& s)
{
	RAMEN_ASSERT( s.x > 0 && s.y > 0);
	proxy_scale_ = s;
}

void image_node_t::set_bounds( const Imath::Box2i& bounds) { bounds_ = bounds;}

void image_node_t::calc_bounds( const render::context_t& context)
{
	if( is_valid_ && !is_identity_)
		do_calc_bounds( context);
	else
		image_node_t::do_calc_bounds( context);
}

void image_node_t::do_calc_bounds( const render::context_t& context)
{
    if( ( num_inputs() != 0) && input_as<image_node_t>())
        set_bounds( input_as<image_node_t>()->bounds());
	else
		set_bounds( format());
}

void image_node_t::clear_interest() { interest_ = Imath::Box2i();}

void image_node_t::set_interest( const Imath::Box2i& roi) { interest_ = roi;}
void image_node_t::add_interest( const Imath::Box2i& roi) { interest_.extendBy( roi);}

void image_node_t::calc_inputs_interest( const render::context_t& context)
{
	if( !is_valid_)
		return;

	if( !is_identity_)
		do_calc_inputs_interest( context);
	else
		image_node_t::do_calc_inputs_interest( context);
}

void image_node_t::do_calc_inputs_interest( const render::context_t& context)
{
    // by default, pass the interest area to the inputs
    BOOST_FOREACH( node_input_plug_t& i, input_plugs())
    {
        if( i.connected())
        {
            if( image_node_t *in = dynamic_cast<image_node_t*>( i.input_node()))
                in->add_interest( interest_);
        }
    }
}

void image_node_t::set_defined( const Imath::Box2i& b) { defined_ = b;}

void image_node_t::calc_defined( const render::context_t& context)
{
	if( !is_valid_)
		defined_ = ImathExt::intersect( format_, interest_);
	else
	{
	    if( is_identity_)
	    {
	        if( num_inputs() != 0 && input_as<image_node_t>())
	            set_defined( input_as<image_node_t>()->defined());
			else
			{
				// this should never happen.
				RAMEN_ASSERT( 0 && "calc_defined: is_identity == true but first input is 0");
			}
	    }
		else
			do_calc_defined( context);
	}

	// limit the image size
	if( defined_.size().x > app().preferences().max_image_width())
		defined_.max.x = defined_.min.x + app().preferences().max_image_width();

	if( defined_.size().y > app().preferences().max_image_height())
		defined_.max.y = defined_.min.y + app().preferences().max_image_height();
}

void image_node_t::do_calc_defined( const render::context_t& context)
{
    defined_ = ImathExt::intersect( bounds_, interest_);
}

void image_node_t::subsample_areas( const render::context_t& context)
{
    if( context.subsample != 1)
    {
        format_   = ImathExt::scale( format_  , 1.0f / context.subsample);
        bounds_   = ImathExt::scale( bounds_  , 1.0f / context.subsample);
        interest_ = ImathExt::scale( interest_, 1.0f / context.subsample);
        defined_  = ImathExt::scale( defined_ , 1.0f / context.subsample);
    }
}

void image_node_t::recursive_calc_format( const render::context_t& context)
{
    depth_first_inputs_search( *this, boost::bind( &image_node_t::calc_format_fun, _1, context));
}

bool image_node_t::use_cache( const render::context_t& context) const { return true;}

bool image_node_t::read_image_from_cache( const render::context_t& context)
{
    if( !cacheable() || !use_cache( context))
	{
		#ifndef NDEBUG
			std::cout << "cache miss, node " << name() << " not cacheable\n" << std::endl;
		#endif

		return false;
	}

    boost::optional<image::buffer_t> cached( app().memory_manager().find_in_cache( digest(), defined()));

    if( cached)
    {
        image_ = cached.get();
        return true;
    }

	#ifndef NDEBUG
		std::cout << "cache miss: " << name() << ", " << hash_generator().digest_as_string()
				  << "\n" << std::endl;
	#endif

    return false;
}

void image_node_t::write_image_to_cache( const render::context_t& context)
{
    if( !cacheable() || !use_cache( context))
        return;

    if( !defined().isEmpty())
        app().memory_manager().insert_in_cache( this, digest(), image_);
}

// images
void image_node_t::alloc_image()
{
    image_ = image::buffer_t( defined(), 4);

    if( !image_.empty())
        boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 0));
}

void image_node_t::release_image()	{ image_ = image::buffer_t();}

image::image_view_t image_node_t::image_view() { return image_.rgba_subimage_view( defined());}

image::const_image_view_t image_node_t::const_image_view() const { return image_.const_rgba_subimage_view( defined());}

image::image_view_t image_node_t::subimage_view( int x, int y, int w, int h)
{
    return image_.rgba_subimage_view( Imath::Box2i( Imath::V2i( x, y), Imath::V2i( w - 1, h - 1)));
}

image::image_view_t image_node_t::subimage_view( const Imath::Box2i& area)
{
    return image_.rgba_subimage_view( area);
}

image::const_image_view_t image_node_t::const_subimage_view( int x, int y, int w, int h) const
{
    return image_.const_rgba_subimage_view( Imath::Box2i( Imath::V2i( x, y), Imath::V2i( w - 1, h - 1)));
}

image::const_image_view_t image_node_t::const_subimage_view( const Imath::Box2i& area) const
{
    return image_.const_rgba_subimage_view( area);
}

void image_node_t::recursive_process( const render::context_t& context)
{
	if( interacting())
	{
		RAMEN_ASSERT( context.mode == render::interface_render);
	}

    if( !is_valid())
    {
        alloc_image();
        image::make_color_bars( image_view());
        return;
    }

    if( is_identity())
    {
		// TODO: Not sure if this is correct, check it.
        if( image_node_t *in = input_as<image_node_t>())
        {
            in->recursive_process( context);
            image_ = in->image_;
            in->release_image();
        }
		
        return;
    }

    if( read_image_from_cache( context))
		return;

    do_recursive_process( context);
	
	if( !context.render_cancelled())
	    write_image_to_cache( context);

    BOOST_FOREACH( node_input_plug_t& i, input_plugs())
    {
        if( i.connected())
        {
            if( image_node_t *in = dynamic_cast<image_node_t*>( i.input_node()))
                in->release_image();
        }
    }
}

void image_node_t::do_recursive_process( const render::context_t& context)
{
	// in this case, you have to manually process the inputs.
	if( !frames_needed().empty())
	{
		if( !context.render_cancelled())
		{
			alloc_image();
			process( context);
		}
		
		return;
	}

	// normal case
	
    // pixels are shared between image buffers.
    // as long as we keep a copy, the pixels won't be deleted
    std::vector<image::buffer_t> buffers;

    BOOST_FOREACH( node_input_plug_t& i, input_plugs())
    {
		if( context.render_cancelled())
			return;
		
        if( i.connected())
        {
            if( image_node_t *in = dynamic_cast<image_node_t*>( i.input_node()))
            {
                if( in->image_empty())
                    in->recursive_process( context);

                buffers.push_back( in->image());
            }
        }
    }

	int j = 0;
	BOOST_FOREACH( node_input_plug_t& i, input_plugs())
	{
		if( i.connected())
		{
			if( image_node_t *in = dynamic_cast<image_node_t*>( i.input_node()))
			{
				if( in->image_empty())
					in->set_image( buffers[j]);
	
				++j;
			}
		}
	}

	if( !context.render_cancelled())
	{
	    alloc_image();
	    process( context);
	}
}

void image_node_t::process( const render::context_t& context)
{
	if( context.render_cancelled())
		return;

    if( !defined().isEmpty())
        do_process( context);
}

void image_node_t::do_process( const render::context_t& context) {}

// used in renderer and other places to recursive calc areas
void image_node_t::calc_format_fun( node_t& n, const render::context_t& context)
{
    if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        in->calc_format( context);
}

void image_node_t::calc_bounds_fun( node_t& n, const render::context_t& context)
{
    if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        in->calc_bounds( context);
}

void image_node_t::clear_interest_fun( node_t& n)
{
    if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        in->clear_interest();
}

void image_node_t::calc_inputs_interest_fun( node_t& n, const render::context_t& context)
{
    if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        in->calc_inputs_interest( context);
}

void image_node_t::calc_defined_fun( node_t& n, const render::context_t& context)
{
    if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        in->calc_defined( context);
}

void image_node_t::subsample_areas_fun( node_t& n, const render::context_t& context)
{
    if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        in->subsample_areas( context);
}

} // namespace
