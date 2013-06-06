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

void composition_settings_dialog_t::exec_dialog()
{
	// update widgets here...
	ui_.format_->set_value( app().document().composition().default_format());
	ui_.rate_->setValue( app().document().composition().frame_rate());
	ui_.ocio_key1_->setText(   QString::fromStdString( app().document().composition().ocio_context_pairs()[0].first));
	ui_.ocio_value1_->setText( QString::fromStdString( app().document().composition().ocio_context_pairs()[0].second));
	ui_.ocio_key2_->setText(   QString::fromStdString( app().document().composition().ocio_context_pairs()[1].first));
	ui_.ocio_value2_->setText( QString::fromStdString( app().document().composition().ocio_context_pairs()[1].second));
	ui_.ocio_key3_->setText(   QString::fromStdString( app().document().composition().ocio_context_pairs()[2].first));
	ui_.ocio_value3_->setText( QString::fromStdString( app().document().composition().ocio_context_pairs()[2].second));
	ui_.ocio_key4_->setText(   QString::fromStdString( app().document().composition().ocio_context_pairs()[3].first));
	ui_.ocio_value4_->setText( QString::fromStdString( app().document().composition().ocio_context_pairs()[3].second));
	
    int result = exec();

    if( result == QDialog::Accepted)
    {
		app().document().composition().set_default_format( ui_.format_->value());
		app().document().composition().set_frame_rate( ui_.rate_->value());
		
		if( app().document().composition().set_ocio_context_key_value( 0, ui_.ocio_key1_->text().toStdString(),
																			ui_.ocio_value1_->text().toStdString()) ||
			app().document().composition().set_ocio_context_key_value( 1, ui_.ocio_key2_->text().toStdString(),
																			ui_.ocio_value2_->text().toStdString()) ||
			app().document().composition().set_ocio_context_key_value( 2, ui_.ocio_key3_->text().toStdString(),
																			ui_.ocio_value3_->text().toStdString()) ||
			app().document().composition().set_ocio_context_key_value( 3, ui_.ocio_key4_->text().toStdString(),
																			ui_.ocio_value4_->text().toStdString()))
		{
			app().ui()->viewer().update_display_transform();
			app().document().composition().ocio_context_changed();
		}
		
		app().document().set_dirty( true);
    }
}

} // namespace
} // namespace
