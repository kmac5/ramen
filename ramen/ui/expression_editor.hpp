// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UI_EXPRESSION_EDITOR_HPP
#define	RAMEN_UI_EXPRESSION_EDITOR_HPP

#include<ramen/python/python.hpp>

#include<boost/noncopyable.hpp>
#include<boost/function.hpp>
#include<boost/python/object.hpp>

#include<QString>

#include<ramen/assert.hpp>

#include<ramen/expressions/se_expression.hpp>

namespace ramen
{
namespace ui
{

struct expression_editor_callback_suite_t
{
	expression_editor_callback_suite_t( const boost::function<void( const std::string&)>& accept_fun,
										const boost::function<void( const std::string&)>& apply_fun,
										const boost::function<void()> cancel_fun)

	{
		RAMEN_ASSERT( accept_fun && apply_fun && cancel_fun);
		accept_fun_ = accept_fun;
		apply_fun_ = apply_fun;
		cancel_fun_ = cancel_fun;
	}

	void accept( const std::string& new_expr)	{ accept_fun_( new_expr);}
	void apply( const std::string& new_expr)	{ apply_fun_( new_expr);}
	void cancel() { cancel_fun_();}

private:

	boost::function<void( const std::string&)> accept_fun_;
	boost::function<void( const std::string&)> apply_fun_;
	boost::function<void()> cancel_fun_;
};

class expression_editor_t : boost::noncopyable
{
public:

    static expression_editor_t& instance();

	void show( expressions::se_expression_t & expr, const expression_editor_callback_suite_t& callbacks);

private:

    expression_editor_t();

	boost::python::object create_expr_editor_;
};

} // namespace
} // namespace

#endif
