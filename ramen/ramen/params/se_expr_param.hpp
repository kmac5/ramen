// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_SE_EXPR_PARAM_HPP
#define	RAMEN_SE_EXPR_PARAM_HPP

#include<ramen/params/static_param.hpp>

#include<boost/function.hpp>

#include<QPoint>
#include<QPointer>

#include<ramen/expressions/se_expression.hpp>

#include<ramen/ui/widgets/text_edit_fwd.hpp>

class QWidget;

namespace ramen
{

class se_expr_param_t : public static_param_t
{
    Q_OBJECT

public:

    typedef boost::function<expressions::se_expression_t*()> create_expr_fun_type;

    se_expr_param_t( const std::string& name, const create_expr_fun_type& create_expr_fun);

    void set_default_value( const std::string& x);
    void set_value( const std::string& x, change_reason reason = user_edited);

    void expr_editor_accept( const std::string& new_expr);
    void expr_editor_apply( const std::string& new_expr);
    void expr_editor_cancel();

protected:

    se_expr_param_t( const se_expr_param_t& other);
    void operator=( const se_expr_param_t& other);

private:

    virtual param_t *do_clone() const { return new se_expr_param_t( *this);}

    virtual void do_add_to_hash( hash::generator_t& hash_gen) const;

    virtual boost::python::object to_python( const poly_param_value_t& v) const;
    virtual poly_param_value_t from_python( const boost::python::object& obj) const;

    virtual void do_read( const serialization::yaml_node_t& node);
    virtual void do_write( serialization::yaml_oarchive_t& out) const;

    virtual QWidget *do_create_widgets() RAMEN_WARN_UNUSED_RESULT;

    virtual void do_update_widgets();
    virtual void do_enable_widgets( bool e);

    QPointer<QWidget> top_widget_;
    QPointer<ui::text_edit_t> multi_input_;
    create_expr_fun_type create_expr_fun_;
    std::string saved_expression_;

private Q_SLOTS:

    void text_changed();
    void show_context_menu( const QPoint& p);
    void show_expression_editor();
};

} // namespace

#endif
