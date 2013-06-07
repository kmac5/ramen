// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOTION_BLUR_PARAM_HPP
#define RAMEN_MOTION_BLUR_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<QPointer>

#include<ramen/params/motion_blur_info.hpp>

#include<ramen/ui/widgets/double_spinbox_fwd.hpp>

class QComboBox;

namespace ramen
{

class RAMEN_API motion_blur_param_t : public static_param_t
{
    Q_OBJECT

public:

    explicit motion_blur_param_t( const std::string& name);

    bool motion_blur_enabled() const;

    motion_blur_info_t::loop_data_t loop_data( float time, int extra_samples, float shutter_factor) const;

protected:

    motion_blur_param_t( const motion_blur_param_t& other);
    void operator=( const motion_blur_param_t& other);

private Q_SLOTS:

    void samples_changed( double v);
    void shutter_changed( double v);
    void filter_changed( int index);

private:

    virtual param_t *do_clone() const { return new motion_blur_param_t( *this);}

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<ui::double_spinbox_t> shutter_, shutter_off_, samples_;
    QPointer<QComboBox> filter_;
};

} // namespace

#endif
