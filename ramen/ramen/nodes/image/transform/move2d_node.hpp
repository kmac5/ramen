// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_MOVE2D_NODE_HPP
#define RAMEN_IMAGE_MOVE2D_NODE_HPP

#include<ramen/nodes/image/xform2d_node.hpp>

namespace ramen
{
namespace image
{

class move2d_node_t : public xform2d_node_t
{
public:

    static const node_metaclass_t& move2d_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    move2d_node_t();

protected:

    move2d_node_t( const move2d_node_t& other);
    void operator=( const move2d_node_t&);

private:

    node_t *do_clone() const { return new move2d_node_t( *this);}

    virtual void do_create_params();
    void param_changed( param_t *p, param_t::change_reason reason);
	
    virtual void do_create_manipulators();

    virtual matrix3_type do_calc_transform_matrix_at_frame( float frame, int subsample = 1) const;
};

} // namespace
} // namespace

#endif
