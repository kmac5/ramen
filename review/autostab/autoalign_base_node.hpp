// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_AUTOALIGN_BASE_NODE_HPP
#define RAMEN_IMAGE_AUTOALIGN_BASE_NODE_HPP

#include<ramen/nodes/image/xform_node.hpp>

#include<boost/optional.hpp>

#include<OpenEXR/ImathMatrix.h>

#include<ramen/params/group_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>

namespace ramen
{
namespace image
{

class autoalign_base_node_t : public xform_node_t
{
public:

    autoalign_base_node_t();

protected:

    autoalign_base_node_t( const autoalign_base_node_t& other);
    void operator=( const autoalign_base_node_t&);

	// similarity
	void create_similarity_params();
	void get_similarity_params();
	void set_similarity_identity( float frame);
	void set_similarity_params( float frame, const Imath::M33d& m);
	Imath::M33d similarity_matrix_at_frame( float frame, int subsample) const;
	
	// util
	Imath::M33d remove_SR( const Imath::M33d& m) const;
	Imath::M33d remove_S( const Imath::M33d& m) const;
	Imath::M33d remove_R( const Imath::M33d& m) const;
	
	// perpective
	void create_perspective_params();	
	void get_perspective_params();
	void set_perspective_identity( float frame);
	void set_perspective_params( float frame, const Imath::M33d& m);
	Imath::M33d perspective_matrix_at_frame( float frame, int subsample) const;

	// corner pin
	void create_corner_pin_params();
	void get_corner_pin_params();
	void set_corner_pin_identity( float frame);
	void set_corner_pin_params( float frame, const Imath::M33d& m);
	Imath::M33d corner_pin_matrix_at_frame( float frame, int subsample) const;

	float_param_t *similarity_[6];
	float_param_t *perspective_[8];
	float2_param_t *corner_[4];
	
private:
	
	float_param_t *create_float_param( group_param_t *g, const std::string& str, float value = 0.0f);
	float2_param_t *create_float2_param( group_param_t *g, const std::string& str, const Imath::V2f& value);	
};

} // namespace
} // namespace

#endif
