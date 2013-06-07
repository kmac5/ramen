// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/dialogs/preferences_dialog.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

preferences_dialog_t& preferences_dialog_t::instance()
{
    static preferences_dialog_t dialog;
    return dialog;
}

preferences_dialog_t::preferences_dialog_t() : QDialog( app().ui()->main_window())
{
    setWindowTitle( "Preferences");
	ui_.setupUi( this);
	
	// connect signals
	connect( ui_.ram_slider_, SIGNAL( valueChanged(int)), ui_.ram_spinbox_, SLOT( setValue(int)));
	connect( ui_.ram_spinbox_, SIGNAL( valueChanged(int)), ui_.ram_slider_, SLOT( setValue(int)));
}

void preferences_dialog_t::exec_dialog()
{
	// update widgets value here...
	ui_.default_format_->set_value( app().preferences().default_format());
	ui_.frame_rate_->setValue( app().preferences().frame_rate());
	ui_.ram_slider_->setValue( app().preferences().max_image_memory());
	ui_.ram_spinbox_->setValue( app().preferences().max_image_memory());

    int result = exec();

    if( result == QDialog::Accepted)
    {
		app().preferences().set_max_image_memory( ui_.ram_spinbox_->value());
		app().preferences().set_default_format( ui_.default_format_->value());
		app().preferences().set_frame_rate( ui_.frame_rate_->value());
		app().preferences().save();
    }
}

} // ui
} // ramen
