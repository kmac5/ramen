// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_SHAPE_TRANSFORM_PARAM_HPP
#define	RAMEN_SHAPE_TRANSFORM_PARAM_HPP

#include<ramen/params/composite_param.hpp>

#include<OpenEXR/ImathMatrix.h>

namespace ramen
{

class shape_transform_param_t : public composite_param_t
{
    Q_OBJECT

public:

    explicit shape_transform_param_t( const std::string& name, const std::string& id);
	
    const param_t& center_param() const;
	void set_center( const Imath::V2f& c, change_reason reason = user_edited);
	
    const param_t& translate_param() const;
    param_t& translate_param();
	void set_translate( const Imath::V2f& t, change_reason reason = user_edited);
	
    const param_t& scale_param() const;
    param_t& scale_param();

    const param_t& rotate_param() const;
    param_t& rotate_param();

	Imath::M33f matrix() const;
    Imath::M33f matrix_at_frame( float frame) const;

protected:

    shape_transform_param_t( const shape_transform_param_t& other);
    void operator=( const shape_transform_param_t& other);

private:

    virtual param_t *do_clone() const { return new shape_transform_param_t( *this);}	
};

} // namespace

#endif
