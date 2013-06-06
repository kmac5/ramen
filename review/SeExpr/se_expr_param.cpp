// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/se_expr_param.hpp>

#include<boost/bind.hpp>

#include<ramen/nodes/node.hpp>

#include<QLabel>
#include<QHBoxLayout>
#include<QMenu>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/text_edit.hpp>

//#include<ramen/ui/expression_editor.hpp>

namespace ramen
{

se_expr_param_t::se_expr_param_t( const std::string& name, const create_expr_fun_type& create_expr_fun) : static_param_t( name)
{
    RAMEN_ASSERT( create_expr_fun);

    create_expr_fun_ = create_expr_fun;
    set_default_value( std::string());
}

se_expr_param_t::se_expr_param_t( const se_expr_param_t& other) : static_param_t( other)
{
    create_expr_fun_ = other.create_expr_fun_;
    top_widget_ = 0;
    multi_input_ = 0;
}

void se_expr_param_t::set_default_value( const std::string& x) { value().assign( x);}

void se_expr_param_t::set_value( const std::string& x, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    value().assign( x);
    emit_param_changed( reason);
}

void se_expr_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    hash_gen << get_value<std::string>( *this);
}

boost::python::object se_expr_param_t::to_python( const poly_param_value_t& v) const
{
    return boost::python::object( v.cast<std::string>());
}

poly_param_value_t se_expr_param_t::from_python( const boost::python::object& obj) const
{
    std::string str = boost::python::extract<std::string>( obj);
    return poly_param_value_t( str);
}

void se_expr_param_t::do_read( const serialization::yaml_node_t& node)
{
    std::string val;
    node.get_value<std::string>( "value", val);
    set_value( val);
}

void se_expr_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    out << YAML::Key << "value"
        << YAML::Value << get_value<std::string>( *this);
}

QWidget *se_expr_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    ui::line_edit_t *tmp = new ui::line_edit_t();
    QSize s = tmp->sizeHint();
    delete tmp;

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());

    int width = app().ui()->inspector().width() - app().ui()->inspector().left_margin() - 10;
    int height = 0;

    std::string str = get_value<std::string>( *this);
    multi_input_ = new ui::text_edit_t( top);
    multi_input_->move( app().ui()->inspector().left_margin(), height);
    multi_input_->resize( width, s.height() * 7);
    multi_input_->setEnabled( enabled());
    multi_input_->setPlainText( str.c_str());

    multi_input_->setContextMenuPolicy( Qt::CustomContextMenu);
    connect( multi_input_, SIGNAL( textHasChanged()), this, SLOT( text_changed()));
    connect( multi_input_, SIGNAL( customContextMenuRequested( const QPoint&)), this, SLOT( show_context_menu( const QPoint&)));
    height += multi_input_->height();

    top->setMinimumSize( app().ui()->inspector().width(), height);
    top->setMaximumSize( app().ui()->inspector().width(), height);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    top_widget_ = top;
    return top;
}

void se_expr_param_t::do_update_widgets()
{
    if( multi_input_)
    {
        multi_input_->blockSignals( true);
        std::string str = get_value<std::string>( *this);
        multi_input_->setPlainText( str.c_str());
        multi_input_->blockSignals( false);
    }
}

void se_expr_param_t::do_enable_widgets( bool e)
{
    if( top_widget_)
        top_widget_->setEnabled( e);
}

void se_expr_param_t::text_changed()
{
    std::string str = multi_input_->toPlainText().toStdString();
    param_set()->begin_edit();
    set_value( str);
    param_set()->end_edit();
}

void se_expr_param_t::show_context_menu( const QPoint& p)
{
    QMenu *menu = new QMenu();
    menu->addAction( "Expression editor...", this, SLOT( show_expression_editor()));
    menu->exec( multi_input_->mapToGlobal( p));
    delete menu;
}

void se_expr_param_t::show_expression_editor()
{
    /*
    saved_expression_ = get_value<std::string>( *this);

    std::auto_ptr<expressions::se_expression_t> expr( create_expr_fun_());

    ui::expression_editor_callback_suite_t cback( boost::bind( &se_expr_param_t::expr_editor_accept, this, _1),
                                                  boost::bind( &se_expr_param_t::expr_editor_apply, this, _1),
                                                  boost::bind( &se_expr_param_t::expr_editor_cancel, this));

    ui::expression_editor_t::instance().show( *(expr.get()), cback);
    */
}

void se_expr_param_t::expr_editor_accept( const std::string& new_expr)
{
    param_set()->begin_edit();
    set_value( new_expr);
    param_set()->end_edit();
    saved_expression_.clear();
    update_widgets();
}

void se_expr_param_t::expr_editor_apply( const std::string& new_expr)
{
    set_value( new_expr);
    param_set()->notify_parent();
    update_widgets();
}

void se_expr_param_t::expr_editor_cancel()
{
    if( get_value<std::string>( *this) != saved_expression_)
    {
        set_value( saved_expression_, node_edited);
        param_set()->notify_parent();
    }

    saved_expression_.clear();
    update_widgets();
}

} // namespace
