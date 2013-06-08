// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLOAT3_PARAM_HPP
#define	RAMEN_FLOAT3_PARAM_HPP

#include<ramen/params/proportional_param.hpp>

#include<OpenEXR/ImathVec.h>

#include<QPointer>

#include<ramen/ui/widgets/param_spinbox_fwd.hpp>

namespace ramen
{

class RAMEN_API float3_param_t : public proportional_param_t
{
    Q_OBJECT

public:

    explicit float3_param_t( const std::string& name);

    void set_default_value( const Imath::V3f& x);

    virtual poly_param_value_t value_at_frame( float frame) const;

    void set_value( const Imath::V3f& x, change_reason reason = user_edited);
    void set_value_at_frame( const Imath::V3f& x, float frame, change_reason reason = user_edited);

    Imath::V3f derive( float time) const;
    Imath::V3f integrate( float time1, float time2) const;

protected:

    float3_param_t( const float3_param_t& other);
    void operator=( const float3_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new float3_param_t( *this);}

    void calc_proportional_factors();
    void set_component_value_from_slot();

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual QWidget *do_create_widgets();
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<ui::param_spinbox_t> input0_, input1_, input2_;

private Q_SLOTS:

    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_dragged( double value);
    void spinbox_released();
};

} // namespace

#endif
