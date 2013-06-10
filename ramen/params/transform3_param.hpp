// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_TRANSFORM3_PARAM_HPP
#define	RAMEN_TRANSFORM3_PARAM_HPP

// avoid a moc error, when including boost
#ifndef QT_MOC_RUN
    #include<ramen/params/composite_param.hpp>

    #include<OpenEXR/ImathMatrix.h>

    #include<ramen/nodes/node_fwd.hpp>

    #include<ramen/image/typedefs.hpp>
#endif

namespace ramen
{

class transform3_param_t : public composite_param_t
{
    Q_OBJECT

public:

	typedef image::matrix4_t matrix_type;
	
    transform3_param_t( const std::string& name, const std::string& id, bool image_mode = true);
	
	bool image_mode() const { return image_mode_;}
	
    const param_t& center_xy_param() const;
    param_t& center_xy_param();

    const param_t& center_z_param() const;
    param_t& center_z_param();

    const param_t& center_param() const;
    param_t& center_param();
	
    const param_t& translate_param() const;
    param_t& translate_param();

    const param_t& scale_param() const;
    param_t& scale_param();

    const param_t& rotate_param() const;
    param_t& rotate_param();

	Imath::V3f get_center() const;
	Imath::V3f get_center_at_frame( float frame) const;
	
    matrix_type matrix_at_frame( float frame, float aspect = 1.0f, int subsample = 1) const;
	
protected:

    transform3_param_t( const transform3_param_t& other);
    void operator=( const transform3_param_t& other);

private:

    virtual param_t *do_clone() const { return new transform3_param_t( *this);}
		
	bool image_mode_;
};

} // namespace

#endif
