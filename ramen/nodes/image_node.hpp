// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_NODE_HPP
#define RAMEN_IMAGE_NODE_HPP

#include<ramen/nodes/node.hpp>

#include<OpenEXR/ImathColor.h>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/image/buffer.hpp>

#include<ramen/ui/palette.hpp>

namespace ramen
{

class RAMEN_API image_node_t : public node_t
{
public:

    image_node_t();

    virtual void cloned();

    // visitor
    virtual void accept( node_visitor& v);

    // connections
    void add_output_plug();

    virtual bool accept_connection( node_t *src, int port) const;

    // format, bounds & aspect	
    const Imath::Box2i& format() const	    { return format_;}
    const Imath::Box2i& full_format() const	{ return full_format_;}
    void set_format( const Imath::Box2i& d);
    virtual void format_changed();

    void calc_format( const render::context_t& context);

    void recursive_calc_format( const render::context_t& context);

	float aspect_ratio() const		{ return aspect_;}
	void set_aspect_ratio( float a);

	const Imath::V2f& proxy_scale() const { return proxy_scale_;}
	void set_proxy_scale( const Imath::V2f& s);
	
    const Imath::Box2i& bounds() const { return bounds_;}
    void set_bounds( const Imath::Box2i& bounds);
    void calc_bounds( const render::context_t& context);

    // interest
    const Imath::Box2i& interest() const   { return interest_;}
    void clear_interest();
    void set_interest( const Imath::Box2i& roi);
    void add_interest( const Imath::Box2i& roi);
    void calc_inputs_interest( const render::context_t& context);

    // defined
    const Imath::Box2i& defined() const	    { return defined_;}
    void set_defined( const Imath::Box2i& b);
    void calc_defined( const render::context_t& context);

    // subsample
    void subsample_areas( const render::context_t& context);

    // if the node is not expensive to compute like simple color corrections,
    // premultiply, ..., then it can return false here and save a bit of memory.
    virtual bool use_cache( const render::context_t& context) const;

    // images
    bool image_empty() const { return image_.empty();}
    image::buffer_t image() const { return image_;}
    void set_image( image::buffer_t img) { image_ = img;}

	// virtual while testing
    virtual void alloc_image();
    virtual void release_image();

    image::image_view_t image_view();
    image::const_image_view_t const_image_view() const;

    image::image_view_t subimage_view( int x, int y, int w, int h);
    image::image_view_t subimage_view( const Imath::Box2i& area);

    image::const_image_view_t const_subimage_view( int x, int y, int w, int h) const;
    image::const_image_view_t const_subimage_view( const Imath::Box2i& area) const;

    // processing
    void recursive_process( const render::context_t& context);
    void process( const render::context_t& context);

    // functors used with the dataflow algorithms
    static void calc_format_fun( node_t& n, const render::context_t& context);
    static void calc_bounds_fun( node_t& n, const render::context_t& context);
    static void clear_interest_fun( node_t& n);
    static void calc_inputs_interest_fun( node_t& n, const render::context_t& context);
    static void calc_defined_fun( node_t& n, const render::context_t& context);
    static void subsample_areas_fun( node_t& n, const render::context_t& context);

protected:

    image_node_t( const image_node_t& other);
    void operator=( const image_node_t&);

    virtual void do_notify();

    virtual void do_recursive_process( const render::context_t& context);

    // cache
    bool read_image_from_cache( const render::context_t& context);
    void write_image_to_cache( const render::context_t& context);

private:

    virtual void do_calc_format( const render::context_t& context);
    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
    virtual void do_calc_defined( const render::context_t& context);

    virtual void do_process( const render::context_t& context);

    Imath::Box2i format_, bounds_, interest_, defined_;
	Imath::Box2i full_format_;
	float aspect_;
	Imath::V2f proxy_scale_;
		
    image::buffer_t image_;
};

} // namespace

#endif
