// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ASPECT_RATIO_PARAM_HPP
#define	RAMEN_ASPECT_RATIO_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<vector>
#include<utility>

#include<QPointer>

#include<ramen/ui/widgets/double_spinbox_fwd.hpp>

class QComboBox;

namespace ramen
{

class RAMEN_API aspect_ratio_param_t : public static_param_t
{
    Q_OBJECT

public:

    explicit aspect_ratio_param_t( const std::string& name);

    void set_default_value( float x);

    void set_value( float x, change_reason reason = user_edited);

    static void init_presets();
    static void add_preset( const std::pair<std::string, float>& p);

protected:

    aspect_ratio_param_t( const aspect_ratio_param_t& other);
    void operator=( const aspect_ratio_param_t& other);

private:

    virtual param_t *do_clone() const { return new aspect_ratio_param_t( *this);}

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    typedef std::vector<std::pair<std::string, float> > presets_type;

    static const presets_type& presets();
    int index_for_value( float x) const;

    static presets_type presets_;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;

    QPointer<QComboBox> menu_;
    QPointer<ui::double_spinbox_t> input_;

private Q_SLOTS:

    void item_picked( int index);
    void value_changed( double value);
};

} // namespace

#endif
