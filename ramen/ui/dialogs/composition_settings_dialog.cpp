// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/dialogs/composition_settings_dialog.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/undo/command.hpp>
#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/viewer/viewer.hpp>

namespace ramen
{
namespace ui
{
namespace
{

class comp_settings_command_t : public undo::command_t
{
public:

    comp_settings_command_t( const image::format_t& new_format,
                             int new_frame_rate) : undo::command_t( "Comp Settings Changed")
    {
        old_format_ = app().document().composition().default_format();
        new_format_ = new_format;
        old_frame_rate_ = app().document().composition().frame_rate();
        new_frame_rate_ = new_frame_rate;
    }

    virtual void undo()
    {
        app().document().composition().set_default_format( old_format_);
        app().document().composition().set_frame_rate( old_frame_rate_);
        undo::command_t::undo();
    }

    virtual void redo()
    {
        app().document().composition().set_default_format( new_format_);
        app().document().composition().set_frame_rate( new_frame_rate_);
        undo::command_t::redo();
    }

private:

    image::format_t old_format_, new_format_;
    int old_frame_rate_, new_frame_rate_;
};

} // unnamed

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

    if( exec() == QDialog::Accepted)
    {
        if( ui_.format_->value() != app().document().composition().default_format() ||
            ui_.rate_->value() != app().document().composition().frame_rate())
        {
            std::auto_ptr<comp_settings_command_t> cmd( new comp_settings_command_t( ui_.format_->value(),
                                                                                     ui_.rate_->value()));
            cmd->redo();
            app().document().undo_stack().push_back( cmd);
            app().ui()->update();
        }
    }
}

} // ui
} // ramen
