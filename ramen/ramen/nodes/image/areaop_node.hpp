// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_AREAOP_NODE_HPP
#define RAMEN_IMAGE_AREAOP_NODE_HPP

#include<ramen/nodes/image_node.hpp>

namespace ramen
{
namespace image
{

class RAMEN_API areaop_node_t : public image_node_t
{
public:

    areaop_node_t();

protected:

    areaop_node_t( const areaop_node_t& other);
    void operator=( const areaop_node_t&);

    virtual bool expand_defined() const;
    
private:

    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
	virtual void do_calc_defined( const render::context_t& context);
	
    virtual void get_expand_radius( int& hradius, int& vradius) const;

    int hradius_, vradius_;
};

} // namespace
} // namespace

#endif
