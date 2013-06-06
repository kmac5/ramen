// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/widgets/ocio_colorspace_combo.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/ocio/manager.hpp>

namespace ramen
{
namespace ui
{

ocio_colorspace_combo_t::ocio_colorspace_combo_t( QWidget *parent) : ocio_combo_t( parent)
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
    int num_color_spaces = config->getNumColorSpaces();

    for( int i = 0; i < num_color_spaces; ++i )
		addItem( config->getColorSpaceNameByIndex( i));

	set_default();
	connect( this, SIGNAL( currentIndexChanged(int)), this, SLOT( combo_index_changed(int)));
}

void ocio_colorspace_combo_t::set_colorspace( const std::string& cs)
{
	int index = index_for_string( cs);
	RAMEN_ASSERT( index != -1);

	current_colorspace_ = cs;
	setCurrentIndex( index);
}

bool ocio_colorspace_combo_t::set_colorspace_or_default( const std::string& cs)
{
	int index = index_for_string( cs);

    if( index != -1)
    {
        current_colorspace_ = cs;
        setCurrentIndex( index);
        return true;
    }
    else
    {
        set_default();
        return false;
    }
}

void ocio_colorspace_combo_t::set_default()
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
    std::string default_cs_name = config->getColorSpace( OCIO::ROLE_SCENE_LINEAR)->getName();

    int index = 0;
    int num_color_spaces = config->getNumColorSpaces();

    for(int i = 0; i < num_color_spaces; i++)
    {
        if( config->getColorSpaceNameByIndex( i) == default_cs_name)
            index = i;
    }

    setCurrentIndex( index);
    current_colorspace_ = default_cs_name;
}

void ocio_colorspace_combo_t::combo_index_changed( int indx)
{
	current_colorspace_ = currentText().toStdString();
	colorspace_changed( current_colorspace_);
}

} // ui
} // ramen
