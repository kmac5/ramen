// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_COMBO_GROUP_PARAM_HPP
#define	RAMEN_COMBO_GROUP_PARAM_HPP

#include<ramen/params/composite_param.hpp>

#include<QPointer>

class QComboBox;
class QStackedWidget;

namespace ramen
{

class RAMEN_API combo_group_param_t : public composite_param_t
{
    Q_OBJECT

public:

    explicit combo_group_param_t( const std::string& name);

    void set_default_value( int x);

    void set_value( int x, change_reason reason = user_edited);

protected:

    combo_group_param_t( const combo_group_param_t& other);
    void operator=( const combo_group_param_t& other);

private:

    virtual param_t *do_clone() const { return new combo_group_param_t( *this);}

    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual std::auto_ptr<undo::command_t> do_create_command();

    virtual boost::python::object to_python( const poly_param_value_t& v) const;
    virtual poly_param_value_t from_python( const boost::python::object& obj) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;

    QPointer<QComboBox> menu_;
    QPointer<QStackedWidget> stack_;

private Q_SLOTS:

    void item_picked( int index);
};

} // namespace

#endif
