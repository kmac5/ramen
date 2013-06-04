// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_TOOL_HPP
#define	RAMEN_KEYER3D_TOOL_HPP

#include<ramen/python/python.hpp>

#include<ramen/nodes/image/key/keyer3d/tool_fwd.hpp>

#include<ramen/ui/tool.hpp>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node_fwd.hpp>

namespace ramen
{
namespace keyer3d
{

class tool_t : public ui::tool_t
{
public:

    tool_t( image::keyer3d_node_t& parent);

    const image::keyer3d_node_t& parent() const	{ return parent_;}
    image::keyer3d_node_t& parent()				{ return parent_;}

private:
	
    image::keyer3d_node_t& parent_;
};

} // namespace
} // namespace

#endif
