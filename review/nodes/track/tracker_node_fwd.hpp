// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_TRACKER_NODE_FWD_HPP
#define	RAMEN_IMAGE_TRACKER_NODE_FWD_HPP

namespace ramen
{
namespace image
{

enum apply_track_mode
{
	track_mode = 0,
	stab_mode
};
		
enum apply_track_use
{
	translate = 0,
	trans_rot,
	trans_scale,
	trans_rot_scale
};
	
class tracker_node_t;

} // namespace
} // namespace

#endif
