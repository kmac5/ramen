// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/params/group_param.hpp>

#include<QGroupBox>
#include<QVBoxLayout>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/group_box.hpp>

namespace ramen
{

group_param_t::group_param_t( const std::string& name) : composite_param_t( name), can_collapse_( true) {}

group_param_t::group_param_t( const group_param_t& other) : composite_param_t( other), group_( 0) {}

QWidget *group_param_t::do_create_widgets()
{
    if( can_collapse_)
        group_ = new ui::group_box_t();
    else
        group_ = new QGroupBox();

    group_->setTitle( name().c_str());
    group_->setAlignment( Qt::AlignLeft);
    create_widgets_inside_widget( group_);

    group_->setMinimumSize( app().ui()->inspector().width(), 20);
    return group_;
}

void group_param_t::do_enable_widgets( bool e)
{
    if( group_)
        group_->setEnabled( e);
}

} // ramen
