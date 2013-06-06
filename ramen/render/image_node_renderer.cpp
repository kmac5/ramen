// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/render/image_node_renderer.hpp>

#include<sstream>

#include<boost/bind.hpp>

#include<ramen/nodes/graph_algorithm.hpp>
#include<ramen/nodes/image_node.hpp>

#include<ramen/app/composition.hpp>

#include<iostream>

namespace ramen
{
namespace
{

std::ostream& print_box( std::ostream& os, const Imath::Box2i& box)
{
    os << "(( " << box.min.x << ", " << box.min.y << "), ( " << box.max.x << ", " << box.max.y << "))";
    return os;
}

struct print_areas
{
    void operator()( ramen::node_t& n) const
    {
        if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        {
            std::cout << in->name() << " ";
            std::cout << "[" << n.hash_generator().digest_as_string() << "]\n";
            std::cout << "format      = "; print_box( std::cout, in->format()) << "\n";
            std::cout << "bounds      = "; print_box( std::cout, in->bounds()) << "\n";
            std::cout << "interest    = "; print_box( std::cout, in->interest()) << "\n";
            std::cout << "defined     = "; print_box( std::cout, in->defined()) << "\n";
			std::cout << "aspect      = " << in->aspect_ratio() << ", ";
			std::cout << "proxy_scale = " << in->proxy_scale() << "\n\n";
        }
    }
};

struct test_empty_images
{
    void operator()( ramen::node_t& n) const
    {
        if( image_node_t *in = dynamic_cast<image_node_t*>( &n))
        {
            if( !in->image_empty())
                std::cout << "node " << in->name() << ": image not empty\n";
        }
    }
};

} // unnamed

namespace render
{

#ifndef NDEBUG
	bool image_node_renderer_t::do_log = true;
#else
	bool image_node_renderer_t::do_log = false;
#endif

image_node_renderer_t::image_node_renderer_t() : has_context_( false), 	render_done_( false) {}

image_node_renderer_t::image_node_renderer_t( const context_t& new_context) : render_done_( false)
{ 
	set_context( new_context);
}

image_node_renderer_t::~image_node_renderer_t()
{
	if( render_done_)
		n_->release_image();	
}

void image_node_renderer_t::set_context( const context_t& context)
{
	has_context_ = true;
    new_context_ = context;
    n_ = dynamic_cast<image_node_t*>( new_context_.result_node);
    RAMEN_ASSERT( n_);
	n_->release_image();
	
    n_->composition()->set_frame( new_context_.frame);
    depth_first_inputs_search( *n_, boost::bind( &image_node_t::calc_format_fun, _1, new_context_));
    depth_first_inputs_search( *n_, boost::bind( &image_node_t::calc_bounds_fun, _1, new_context_));
    depth_first_inputs_search( *n_, boost::bind( &image_node_t::clear_interest_fun, _1));
}

const Imath::Box2i& image_node_renderer_t::format() const
{
	RAMEN_ASSERT( has_context_);
	return n_->format();
}

const Imath::Box2i& image_node_renderer_t::bounds() const
{
	RAMEN_ASSERT( has_context_);
	return n_->bounds();
}

void image_node_renderer_t::render() { render( format());}

void image_node_renderer_t::render( const Imath::Box2i& roi)
{
	RAMEN_ASSERT( has_context_);
	RAMEN_ASSERT( !roi.isEmpty());

    n_->set_interest( roi);
    breadth_first_inputs_apply( *n_, boost::bind( &image_node_t::calc_inputs_interest_fun, _1, new_context_));
    depth_first_inputs_search(  *n_, boost::bind( &image_node_t::calc_defined_fun, _1, new_context_));
    depth_first_inputs_search(  *n_, boost::bind( &image_node_t::subsample_areas_fun, _1, new_context_));

    if( do_log)
        depth_first_inputs_search( *n_, test_empty_images());

    depth_first_inputs_search( *n_, boost::bind( &node_t::clear_hash, _1));
    depth_first_inputs_search( *n_, boost::bind( &node_t::calc_hash_str, _1, new_context_));

    if( do_log)
        depth_first_inputs_search( *n_, print_areas());

	try
	{
	    n_->recursive_process( new_context_);
		render_done_ = true;
	}
	catch( std::bad_alloc&)
	{
		throw boost::enable_error_info( std::bad_alloc());
	}
}

image::buffer_t image_node_renderer_t::image()
{
	RAMEN_ASSERT( has_context_);
	return n_->image();
}

image::const_image_view_t image_node_renderer_t::format_image_view() const
{
	RAMEN_ASSERT( has_context_);
    return n_->const_subimage_view( format());
}

} // namespace
} // namespace
