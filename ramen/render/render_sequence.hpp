// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_RENDER_SEQUENCE_HPP
#define	RAMEN_RENDER_SEQUENCE_HPP

#include<vector>

#include<boost/foreach.hpp>

#include<ramen/app/composition.hpp>

#include<ramen/nodes/node_output_interface.hpp>

namespace ramen
{
namespace render
{

template<class F>
F for_each_output( composition_t& comp, bool selected_only, F fun)
{
	BOOST_FOREACH( node_t& n, comp.nodes())
	{
		if( node_output_interface *out = dynamic_cast<node_output_interface*>( &n))
		{
			if( ( selected_only && !n.selected()) || n.ignored())
				continue;
	
			fun( out);
		}
	}
	
	return fun;
}
	
int count_output_nodes( composition_t& comp, bool selected_only = false);
void get_output_nodes( composition_t& comp, std::vector<node_output_interface*> outputs, bool selected_only = false);

int total_frames_to_render( composition_t& comp, int start, int end, bool selected_only = false);

void render_sequence( composition_t& comp, int start, int end, int proxy_level,
					  int subsample, int mb_extra_samples, float mb_shutter_factor);

} // namespace
} // namespace

#endif
