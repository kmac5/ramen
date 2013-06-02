// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/render/context_guard.hpp>

#include<boost/bind.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/nodes/graph_algorithm.hpp>
#include<ramen/nodes/image_node.hpp>

namespace ramen
{

namespace render
{

context_guard_t::context_guard_t( const context_t& context, node_t *n)
{
	RAMEN_ASSERT( context.composition);
	context_ = context;
	n_ = n;

	if( n_)
		save( n_);
	else
	{
		for( composition_t::const_node_iterator it( context_.composition->nodes().begin()); it != context_.composition->nodes().end(); ++it)
		{
			const node_t *n = &(*it);
			
			if( n->num_outputs() == 0)
				save( n);
		}
	}
}

context_guard_t::~context_guard_t()
{
	context_.composition->set_frame( context_.frame);
	
	if( n_)
		restore( n_);
	else
	{
		for( composition_t::node_iterator it( context_.composition->nodes().begin()); it != context_.composition->nodes().end(); ++it)
		{
            node_t *n = &(*it);

			if( n->num_outputs() == 0)
				restore( n);
		}	
	}
}

void context_guard_t::save( const node_t *n)
{
	if( const image_node_t *inode = dynamic_cast<const image_node_t*>( n))
	{
		saved_info_t s;
		s.roi = inode->interest();
		saved_[n] = s;
	}	
}

void context_guard_t::restore( node_t *n)
{	
	if( image_node_t *inode = dynamic_cast<image_node_t*>( n))
	{
		depth_first_inputs_search( *inode, boost::bind( &image_node_t::calc_format_fun, _1, context_));
		depth_first_inputs_search( *inode, boost::bind( &image_node_t::calc_bounds_fun, _1, context_));
		depth_first_inputs_search( *inode, boost::bind( &image_node_t::clear_interest_fun, _1));
		
		inode->set_interest( saved_[n].roi);

		breadth_first_inputs_apply( *inode, boost::bind( &image_node_t::calc_inputs_interest_fun, _1, context_));
		depth_first_inputs_search(  *inode, boost::bind( &image_node_t::calc_defined_fun, _1, context_));
		depth_first_inputs_search(  *inode, boost::bind( &image_node_t::subsample_areas_fun, _1, context_));
		depth_first_inputs_search( *inode, boost::bind( &node_t::clear_hash, _1));
		depth_first_inputs_search( *inode, boost::bind( &node_t::calc_hash_str, _1, context_));
	}
}

} // namespace
} // namespace
