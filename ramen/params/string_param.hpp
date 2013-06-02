// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_STRING_PARAM_HPP
#define	RAMEN_STRING_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<QPointer>

#include<ramen/ui/widgets/line_edit_fwd.hpp>
#include<ramen/ui/widgets/text_edit_fwd.hpp>

namespace ramen
{

class string_param_t : public static_param_t
{
    Q_OBJECT

public:

    explicit string_param_t( const std::string& name);
    string_param_t( const std::string& name, bool read_only);

    bool read_only() const { return read_only_;}

    bool multiline() const		{ return multiline_;}
    void set_multiline( bool b)	{ multiline_ = b;}

    void set_default_value( const std::string& x);

    void set_value( const std::string& x, change_reason reason = user_edited);

protected:

    string_param_t( const string_param_t& other);
    void operator=( const string_param_t& other);

private:

    virtual param_t *do_clone() const { return new string_param_t( *this);}

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual boost::python::object to_python( const poly_param_value_t& v) const;
    virtual poly_param_value_t from_python( const boost::python::object& obj) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;
    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<ui::line_edit_t> input_;
    QPointer<ui::text_edit_t> multi_input_;
    bool read_only_;
    bool multiline_;

private Q_SLOTS:

    void text_changed();
};

} // namespace

#endif
