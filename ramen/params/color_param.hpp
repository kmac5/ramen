// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_COLOR_PARAM_HPP
#define	RAMEN_COLOR_PARAM_HPP

#include<ramen/params/animated_param.hpp>

#include<OpenEXR/ImathColor.h>

#include<QPointer>
#include<QToolButton>

#include<ramen/ui/widgets/color.hpp>
#include<ramen/ui/widgets/param_spinbox_fwd.hpp>
#include<ramen/ui/widgets/color_button_fwd.hpp>
#include<ramen/ui/widgets/eyedropper_button_fwd.hpp>

namespace ramen
{

class RAMEN_API color_param_t : public animated_param_t
{
    Q_OBJECT

public:

    explicit color_param_t( const std::string& name);

    void set_default_value( const Imath::Color4f& x);

    virtual poly_param_value_t value_at_frame( float frame) const;

    void set_value( const Imath::Color4f& x, change_reason reason = user_edited);
    void set_value_at_frame( const Imath::Color4f& x, float frame, change_reason reason = user_edited);

    bool is_rgba() const	{ return is_rgba_;}
    void set_is_rgba( bool b)	{ is_rgba_ = b;}

protected:

    color_param_t( const color_param_t& other);
    void operator=( const color_param_t& other);

private:

    void private_init();

    virtual param_t *do_clone() const { return new color_param_t( *this);}

    virtual void do_create_tracks( anim::track_t *parent);

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual boost::python::object to_python( const poly_param_value_t& v) const;
    virtual poly_param_value_t from_python( const boost::python::object& obj) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    bool is_rgba_;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;

    void set_component_value_from_slot();

   QPointer<ui::param_spinbox_t> input0_, input1_, input2_, input3_;
   QPointer<ui::color_button_t> button_;
   QPointer<ui::eyedropper_button_t> eyedropper_;

private Q_SLOTS:

    void value_changed( double value);
    void spinbox_pressed();
    void spinbox_dragged( double value);
    void spinbox_released();
    void expression_set();
    void eyedropper_color_picked( const ramen::ui::color_t& c);
    void color_button_pressed();
};

} // namespace

#endif
