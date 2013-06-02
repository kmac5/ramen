// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_RETIME_NODE_HPP
#define	RAMEN_IMAGE_RETIME_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class retime_node_t : public image_node_t
{
public:

    static const node_metaclass_t& retime_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    retime_node_t();
	
    virtual void release_image();
	
protected:

    retime_node_t( const retime_node_t& other);
    void operator=( const retime_node_t&);

private:

    node_t *do_clone() const { return new retime_node_t( *this);}

    virtual void do_create_params();
	
    virtual bool do_is_valid() const;
	virtual bool do_is_identity() const;

	virtual bool include_input_in_hash( int num) const;
	
	virtual void do_calc_frames_needed( const render::context_t& context);
	
	virtual void do_calc_bounds( const render::context_t& context);
	virtual void do_calc_inputs_interest( const render::context_t& context);
	
    virtual void do_recursive_process( const render::context_t& context);

    void do_retime( const render::context_t& context);
		
	// util functions
	float get_src_frame() const;
	Imath::Box2i get_input_bounds( const render::context_t& context);

	image::buffer_t get_input_image( const render::context_t& context);
	image::buffer_t get_input_image( const render::context_t& context, const Imath::Box2i& roi);
	
	Imath::Box2i full_interest_;
};

} // namespace
} // namespace

#endif
