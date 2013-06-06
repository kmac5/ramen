// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/widgets/ocio_display_combo.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/ocio/manager.hpp>

namespace ramen
{
namespace ui
{

ocio_display_combo_t::ocio_display_combo_t( QWidget *parent) : ocio_combo_t( parent)
{
	ocio::manager_t& manager( app().ocio_manager());

    int num_device_names = manager.displays().size();

    for(int i = 0; i < num_device_names; i++)
        addItem( manager.displays()[i].c_str());

	set_default();
	connect( this, SIGNAL( currentIndexChanged(int)), this, SLOT( combo_index_changed(int)));
}

void ocio_display_combo_t::set_display( const std::string& cs)
{
	int index = index_for_string( cs);
	RAMEN_ASSERT( index != -1);

	current_display_ = cs;
	setCurrentIndex( index);
}

void ocio_display_combo_t::set_default()
{
	ocio::manager_t& manager( app().ocio_manager());
	current_display_ = manager.default_display();
	setCurrentIndex( manager.default_display_index());
}

void ocio_display_combo_t::combo_index_changed( int indx)
{
	current_display_ = currentText().toStdString();
	display_changed( current_display_);
}

} // ui
} // ramen
