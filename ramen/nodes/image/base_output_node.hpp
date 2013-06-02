// Copyright (c) 2010 Esteban Tovagliari.

#ifndef RAMEN_IMAGE_BASE_OUTPUT_NODE_HPP
#define	RAMEN_IMAGE_BASE_OUTPUT_NODE_HPP

#include<ramen/nodes/image_node.hpp>
#include<ramen/nodes/node_output_interface.hpp>

#include<OpenEXR/ImathColor.h>

namespace ramen
{
namespace image
{

class RAMEN_API base_output_node_t : public image_node_t, public node_output_interface
{
public:

    base_output_node_t();

	virtual void process_and_write( const render::context_t& context);

	// by default, output nodes doesn't use the cache.	
    virtual bool use_cache( const render::context_t& context) const { return false;}
	
protected:

    base_output_node_t( const base_output_node_t& other);
    void operator=( const base_output_node_t&);

    virtual void do_process( const render::context_t& context);

	const Imath::Box2i input_defined() const		{ return input_defined_;}
	
private:
	
    virtual void write( const render::context_t& context) = 0;
	
    Imath::Box2i input_defined_;
};

} // namespace
} // namespace

#endif
