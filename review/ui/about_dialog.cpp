// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/dialogs/about_dialog.hpp>

namespace ramen
{
namespace ui
{

about_dialog_t& about_dialog_t::instance()
{
    static about_dialog_t dialog;
    return dialog;
}

about_dialog_t::about_dialog_t() : QDialog( app().ui()->main_window())
{
	ui_.setupUi( this);
	// setup version string here.
}

} // namespace
} // namespace
