// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_PERCENTILE_FILTER_NODE_HPP
#define	RAMEN_IMAGE_PERCENTILE_FILTER_NODE_HPP

#include<ramen/nodes/image/areaop_node.hpp>

namespace ramen
{
namespace image
{

class percentile_filter_node_t : public areaop_node_t
{
public:

    static const node_metaclass_t& percentile_filter_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    percentile_filter_node_t();

protected:

    percentile_filter_node_t( const percentile_filter_node_t& other) : areaop_node_t( other) {}
    void operator=( const percentile_filter_node_t&);

private:

    node_t *do_clone() const { return new percentile_filter_node_t( *this);}

    virtual void do_create_params();

    virtual void do_process( const render::context_t& context);

    virtual void get_expand_radius( int& hradius, int& vradius) const;
};

} // namespace
} // namespace

#endif
