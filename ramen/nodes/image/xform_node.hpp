// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_XFORM_NODE_HPP
#define RAMEN_XFORM_NODE_HPP

#include<ramen/nodes/image/base_warp_node.hpp>

#include<vector>

#include<ramen/assert.hpp>

#include<ramen/params/motion_blur_param.hpp>

namespace ramen
{
namespace image
{

class RAMEN_API xform_node_t : public base_warp_node_t
{
public:
	
	template<class XFormNode>
	image_node_t *get_img_source()
	{
		image_node_t *source = input_as<image_node_t>();
	
		while( 1)
		{
			XFormNode *x = dynamic_cast<XFormNode*>( source);
	
			if( !x)
				break;
	
			if( !source->input_as<image_node_t>())
				break;
	
			source = source->input_as<image_node_t>();
		}
	
		return source;
	}

	template<class XFormNode>
	const image_node_t *get_img_source() const
	{
		const image_node_t *source = input_as<const image_node_t>();
	
		while( 1)
		{
			const XFormNode *x = dynamic_cast<const XFormNode*>( source);
	
			if( !x)
				break;
	
			if( !source->input_as<const image_node_t>())
				break;
	
			source = source->input_as<const image_node_t>();
		}

		return source;
	}
	
	template<class XFormNode>
	const image_node_t *get_img_source_and_xform_list( std::vector<const XFormNode*>& xforms) const
	{
		const XFormNode *self = dynamic_cast<const XFormNode*>( this);
		RAMEN_ASSERT( self);
		
		xforms.push_back( self);

		const image_node_t *source = input_as<image_node_t>();
	
		while( 1)
		{
			const XFormNode *x = dynamic_cast<const XFormNode*>( source);
	
			if( !x)
				break;
	
			xforms.push_back( x);
	
			if( !source->input_as<image_node_t>())
				break;
	
			source = source->input_as<image_node_t>();
		}
	
		return source;
	}
	
protected:

    xform_node_t();
    xform_node_t( const xform_node_t& other);
	
    void operator=( const xform_node_t&);

	void create_motion_blur_param();

	motion_blur_info_t::loop_data_t motion_blur_loop_data( float time, int extra_samples, float shutter_factor) const;

	virtual void do_calc_hash_str( const render::context_t& context);	

private:

	virtual bool do_is_valid() const;

	// interaction
	virtual void do_begin_interaction();
};

} // namespace
} // namespace

#endif
