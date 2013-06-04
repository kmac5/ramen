// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_ALPHA_TRIMAP_NODE_HPP
#define	RAMEN_IMAGE_ALPHA_TRIMAP_NODE_HPP

#include<ramen/nodes/image/areaop_node.hpp>

namespace ramen
{
namespace image
{

class trimap_node_t : public areaop_node_t
{
public:

    static const node_metaclass_t& trimap_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    trimap_node_t();

protected:

    trimap_node_t( const trimap_node_t& other) : areaop_node_t( other) {}
    void operator=( const trimap_node_t&);

    virtual bool expand_defined() const;

private:

    node_t *do_clone() const { return new trimap_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::context_t& context);

    virtual void get_expand_radius( int& hradius, int& vradius) const;
	
	void grow_trimap( std::vector<Imath::V2i>& pts);
};

} // namespace
} // namespace

#endif
