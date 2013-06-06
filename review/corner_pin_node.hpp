// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_CORNER_PIN_NODE_HPP
#define RAMEN_IMAGE_CORNER_PIN_NODE_HPP

#include<ramen/nodes/image/xform2d_node.hpp>

namespace ramen
{
namespace image
{

class corner_pin_node_t : public xform2d_node_t
{
public:

    static const node_metaclass_t& corner_pin_node_metaclass();
    virtual const node_metaclass_t *metaclass() const;

    corner_pin_node_t();

protected:

    corner_pin_node_t( const corner_pin_node_t& other);
    void operator=( const corner_pin_node_t&);

private:

    node_t *do_clone() const { return new corner_pin_node_t(*this);}

    virtual void do_create_params();
    void param_changed( param_t *p, param_t::change_reason reason);
	
	virtual void do_create_manipulators();

    virtual matrix3_type do_calc_transform_matrix_at_frame( float frame, int subsample = 1) const;

    void get_corners( boost::array<Imath::V2d, 4>& pts, float scale = 1.0f) const;
    void get_corners_at_frame( boost::array<Imath::V2d, 4>& pts, float frame, float scale = 1.0f) const;
};

} // namespace
} // namespace

#endif
