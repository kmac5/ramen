// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/dialogs/smooth_anim_curves_dialog.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

namespace ramen
{
namespace ui
{

smooth_anim_curves_dialog_t& smooth_anim_curves_dialog_t::instance()
{
    static smooth_anim_curves_dialog_t dialog;
    return dialog;
}

smooth_anim_curves_dialog_t::smooth_anim_curves_dialog_t() : QDialog( app().ui()->main_window())
{
	ui_.setupUi( this);
}

bool smooth_anim_curves_dialog_t::exec( bool high_pass, float& filter_size, bool& resample)
{
	if( high_pass)
		setWindowTitle( "High Pass");
	else
		setWindowTitle( "Smooth");

	int result = QDialog::exec();
	
	if( result == QDialog::Accepted)
	{
		filter_size = ui_.filter_size_->value();
		resample = ui_.resample_->isChecked();
		return true;
	}
	
	return false;
}

} // namespace
} // namespace
