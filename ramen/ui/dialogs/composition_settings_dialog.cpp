// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/dialogs/composition_settings_dialog.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/viewer/viewer.hpp>

namespace ramen
{
namespace ui
{

composition_settings_dialog_t& composition_settings_dialog_t::instance()
{
    static composition_settings_dialog_t dialog;
    return dialog;
}

composition_settings_dialog_t::composition_settings_dialog_t() : QDialog( app().ui()->main_window())
{
    setWindowTitle( "Composition Settings");
    ui_.setupUi( this);
}

void composition_settings_dialog_t::dialog_changed()	{ dialog_dirty = true;}

void composition_settings_dialog_t::exec_dialog()
{
    dialog_dirty = false;

    // update widgets here...
    ui_.format_->set_value( app().document().composition().default_format());
    ui_.rate_->setValue( app().document().composition().frame_rate());

    connect( ui_.format_, SIGNAL( value_changed()), this, SLOT( dialog_changed()));

    int result = exec();

    if( result == QDialog::Accepted && dialog_dirty)
    {
        app().document().composition().set_default_format( ui_.format_->value());
        app().document().composition().set_frame_rate( ui_.rate_->value());
        app().document().set_dirty( true);
        app().ui()->update();
    }
}

} // ui
} // ramen
