// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_NOP_NODE_HPP
#define RAMEN_IMAGE_NOP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class nop_node_t : public image_node_t
{
public:

    static const node_metaclass_t& nop_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    nop_node_t();
	
    virtual bool use_cache( const render::context_t& context) const { return false;}

protected:

    nop_node_t( const nop_node_t& other) : image_node_t(other) {}
    void operator=( const nop_node_t&);

private:

    node_t *do_clone() const { return new nop_node_t(*this);}

	virtual bool do_is_identity() const { return true;}
};

} // namespace
} // namespace

#endif
