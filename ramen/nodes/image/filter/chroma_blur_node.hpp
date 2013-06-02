// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_CHROMA_BLUR_NODE_HPP
#define	RAMEN_IMAGE_CHROMA_BLUR_NODE_HPP

#include<ramen/nodes/image/areaop_node.hpp>

namespace ramen
{
namespace image
{

class chroma_blur_node_t : public areaop_node_t
{
public:

    static const node_metaclass_t& chroma_blur_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    chroma_blur_node_t();

protected:

    chroma_blur_node_t( const chroma_blur_node_t& other) : areaop_node_t( other) {}
    void operator=( const chroma_blur_node_t&);

private:

    node_t *do_clone() const { return new chroma_blur_node_t( *this);}

	virtual void do_create_params();
	
    virtual void do_process( const render::context_t& context);
    
    virtual void get_expand_radius( int& hradius, int& vradius) const;
};

} // namespace
} // namespace

#endif
