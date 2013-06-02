// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_ADD_GRAIN_NODE_HPP
#define RAMEN_ADD_GRAIN_NODE_HPP

#include<ramen/nodes/image/pointop_node.hpp>

namespace ramen
{
namespace image
{

class add_grain_node_t : public pointop_node_t
{
public:

    static const node_metaclass_t& add_grain_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    add_grain_node_t();

protected:

    add_grain_node_t( const add_grain_node_t& other) : pointop_node_t( other) {}
    void operator=( const add_grain_node_t&);

private:

    node_t *do_clone() const { return new add_grain_node_t(*this);}

    virtual void do_create_params();

	virtual bool do_is_identity() const;
	
    virtual void do_calc_hash_str( const render::context_t& context);

    virtual void do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::context_t& context);

	Imath::V3f channel_offset( const render::context_t& context) const;
};

} // namespace
} // namespace

#endif
