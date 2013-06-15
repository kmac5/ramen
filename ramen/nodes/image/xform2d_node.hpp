// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_XFORM2D_NODE_HPP
#define RAMEN_XFORM2D_NODE_HPP

#include<ramen/nodes/image/xform_node.hpp>

#include<boost/array.hpp>

#include<ramen/image/generic/samplers.hpp>

namespace ramen
{
namespace image
{

class RAMEN_API xform2d_node_t : public xform_node_t
{	
public:
	
	typedef image::vector2_t	vector2_type;
	typedef image::matrix3_t	matrix3_type;
	
protected:

    xform2d_node_t();
	
    xform2d_node_t(const xform2d_node_t& other) : xform_node_t(other) {}
    void operator=(const xform2d_node_t&);
	
    matrix3_type global_matrix_at_frame( float frame, const std::vector<const xform2d_node_t*>& xforms, int subsample = 1) const;

	bool quad_is_convex(const boost::array<vector2_type,4>& quad) const;

    Imath::Box2i transform_box( const Imath::Box2i& box, const matrix3_type& m, int step, bool round_up = false) const;

private:

    virtual matrix3_type calc_transform_matrix_at_frame( float frame, int subsample = 1) const;
    virtual matrix3_type do_calc_transform_matrix_at_frame( float frame, int subsample = 1) const = 0;

    virtual void do_calc_bounds( const render::context_t& context);
    virtual void do_calc_inputs_interest( const render::context_t& context);
	
    virtual void do_recursive_process( const render::context_t& context);

    virtual void do_process( const render::context_t& context);
    void do_process( const render::context_t& context, const image_node_t *src, const image::image_view_t& dst, const matrix3_type& xf);

	void do_process_mipmap( const render::context_t& context);
    void do_process_mipmap( const render::context_t& context, const image_node_t *src, const image::image_view_t& dst,
							const matrix3_type& xf, mipmap_type& sampler);

public:

    matrix3_type global_matrix( int subsample = 1) const;
    matrix3_type global_matrix_at_frame( float frame, int subsample = 1) const;
};

} // image
} // ramen

#endif
