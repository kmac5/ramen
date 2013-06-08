// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLOAT_PARAM_HPP
#define	RAMEN_FLOAT_PARAM_HPP

#include<ramen/params/numeric_param.hpp>

#include<QPointer>

#include<ramen/ui/widgets/param_spinbox_fwd.hpp>

namespace ramen
{

class RAMEN_API float_param_t : public numeric_param_t
{
    Q_OBJECT

public:

    explicit float_param_t( const std::string& name);

    void set_default_value( float x);

    virtual poly_param_value_t value_at_frame( float frame) const;

    void set_value( float x, change_reason reason = user_edited);
    void set_value_at_frame( float x, float frame, change_reason reason = user_edited);

    void set_absolute_value( float x, change_reason reason = user_edited);
    void set_absolute_value_at_frame( float x, float frame, change_reason reason = user_edited);

    float derive( float time) const;
    float integrate( float time1, float time2) const;

protected:

    float_param_t( const float_param_t& other);
    void operator=( const float_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new float_param_t( *this);}

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual void do_format_changed( const Imath::Box2i& new_domain, float aspect, const Imath::V2f& proxy_scale);

    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    virtual QWidget *do_create_widgets();

    QPointer<ui::param_spinbox_t> input_;

private Q_SLOTS:

    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_dragged( double value);
    void spinbox_released();
};

} // namespace

#endif
