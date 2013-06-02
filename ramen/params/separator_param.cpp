// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/separator_param.hpp>

#include<QFrame>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>

namespace ramen
{

separator_param_t::separator_param_t() : param_t() { set_static( true);}

separator_param_t::separator_param_t( const std::string& name) : param_t( name) { set_static( true);}

separator_param_t::separator_param_t( const separator_param_t& other) : param_t( other) {}

QWidget *separator_param_t::do_create_widgets()
{
    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
    separator->setLineWidth( 1);
    QSize s = separator->sizeHint();

    separator->setMinimumSize( app().ui()->inspector().width(), s.height());
    separator->setMaximumSize( app().ui()->inspector().width(), s.height());
    separator->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return separator;
}

} // namespace
