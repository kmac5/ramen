// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/dialogs/render_flipbook_dialog.hpp>

#include<ramen/app/application.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>

#include<ramen/flipbook/factory.hpp>

namespace ramen
{
namespace ui
{

render_flipbook_dialog_t& render_flipbook_dialog_t::instance()
{
    static render_flipbook_dialog_t dialog;
    return dialog;
}

render_flipbook_dialog_t::render_flipbook_dialog_t() : QDialog( app().ui()->main_window())
{
	ui_.setupUi( this);
	
	// init flipbook list
	{
		int index = 0;
		QStringList slist;

		std::string default_flipbook = app().preferences().default_flipbook();
		
		for( int i = 0; i < flipbook::factory_t::instance().flipbooks().size(); ++i)
		{
			std::string fname = flipbook::factory_t::instance().flipbooks()[i].first;
			slist << fname.c_str();
			
			if( fname == default_flipbook)
				index = i;
		}
		
		ui_.flipbook_combo_->addItems( slist);
		ui_.flipbook_combo_->setCurrentIndex( index);
	}

	// ocio
    get_display_devices();
    get_display_transforms();
	connect( ui_.ocio_device_combo_, SIGNAL( activated( int)), this, SLOT( change_display_device( int)));
	connect( ui_.ocio_transform_combo_, SIGNAL( activated( int)), this, SLOT( change_display_transform( int)));
}

int render_flipbook_dialog_t::start_frame() const{ return ui_.range_start_->value();}
int render_flipbook_dialog_t::end_frame() const	{ return ui_.range_end_->value();}
int render_flipbook_dialog_t::resolution() const	{ return ui_.resolution_popup_->currentIndex() + 1;}
int render_flipbook_dialog_t::proxy_level() const{ return ui_.proxy_popup_->currentIndex();}

std::string render_flipbook_dialog_t::flipbook() const
{
	return ui_.flipbook_combo_->currentText().toStdString();
}

int render_flipbook_dialog_t::mblur_extra_samples() const     { return ui_.mblur_extra_samples_->value();}
float render_flipbook_dialog_t::mblur_shutter_factor() const  { return ui_.mblur_shutter_factor_->value();}

void render_flipbook_dialog_t::set_frame_range( int lo, int hi)
{
    ui_.range_start_->setValue( lo);
    ui_.range_end_ ->setValue( hi);
}

void render_flipbook_dialog_t::get_display_devices()
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

    int index = 0;
    QStringList slist;

    std::string default_device_name = config->getDefaultDisplay();
    int num_device_names = config->getNumDisplays();

    for(int i = 0; i < num_device_names; i++)
    {
        std::string devname = config->getDisplay( i);
        display_devices_.push_back( devname);

        slist << devname.c_str();

        if( default_device_name == devname)
            index = i;
    }

    ui_.ocio_device_combo_->addItems( slist);
    ui_.ocio_device_combo_->setCurrentIndex( index);
    display_device_ = default_device_name;
}

void render_flipbook_dialog_t::get_display_transforms()
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

    std::string device_name = display_devices_[ ui_.ocio_device_combo_->currentIndex()];
    std::string default_transform = config->getDefaultView( device_name.c_str());

    int index = 0;
    QStringList slist;
    int num_transform_names = config->getNumViews( device_name.c_str());

    for( int i = 0; i < num_transform_names; ++i)
    {
        std::string trname = config->getView( device_name.c_str(), i);
        display_transforms_.push_back( trname);
        slist << trname.c_str();

        if( default_transform == trname)
            index = i;
    }

    ui_.ocio_transform_combo_->addItems( slist);
    ui_.ocio_transform_combo_->setCurrentIndex( index);
    display_transform_ = default_transform;
}

const std::string& render_flipbook_dialog_t::display_device() const      { return display_device_;}
const std::string& render_flipbook_dialog_t::display_transform() const   { return display_transform_;}

void render_flipbook_dialog_t::change_display_device( int index)
{
    display_device_ = display_devices_[index];

    // refresh the list of transforms
    ui_.ocio_transform_combo_->blockSignals( true);
    ui_.ocio_transform_combo_->clear();

    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

    std::string default_transform = config->getDefaultView( display_device_.c_str());
    int num_transform_names = config->getNumViews( display_device_.c_str());

    int default_index = 0, new_index = -1;
    QStringList slist;

    for( int i = 0; i < num_transform_names; ++i)
    {
        std::string trname = config->getView( display_device_.c_str(), i);
        display_transforms_.push_back( trname);
        slist << trname.c_str();

        if( display_transform_ == trname)
            new_index = i;

        if( default_transform == trname)
            default_index = i;
    }

    ui_.ocio_transform_combo_->addItems( slist);

    if( new_index < 0)
        new_index = default_index;

    ui_.ocio_transform_combo_->setCurrentIndex( new_index);
    display_transform_ = ui_.ocio_transform_combo_->itemText( new_index).toStdString();
    ui_.ocio_transform_combo_->blockSignals( false);
}

void render_flipbook_dialog_t::change_display_transform( int index)
{
    display_transform_ = display_transforms_[index];
}

} // namespace
} // namespace
