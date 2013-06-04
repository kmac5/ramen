// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_GAUSS_BLUR_NODE_HPP
#define	RAMEN_IMAGE_GAUSS_BLUR_NODE_HPP

#include<ramen/nodes/image/base_blur_node.hpp>

namespace ramen
{
namespace image
{

class gauss_blur_node_t : public base_blur_node_t
{
public:

    static const node_metaclass_t& gauss_blur_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    gauss_blur_node_t();

protected:

    gauss_blur_node_t( const gauss_blur_node_t& other) : base_blur_node_t( other) {}
    void operator=( const gauss_blur_node_t&);

private:

    node_t *do_clone() const { return new gauss_blur_node_t( *this);}

    virtual void do_create_params();

	virtual bool do_is_identity() const;
	
    virtual void do_process( const render::context_t& context);

    virtual void get_expand_radius( int& hradius, int& vradius) const;
};

} // namespace
} // namespace

#endif

