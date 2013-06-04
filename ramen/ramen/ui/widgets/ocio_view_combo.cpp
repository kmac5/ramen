// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/widgets/ocio_view_combo.hpp>

#include<ramen/assert.hpp>

#include<ramen/app/application.hpp>
#include<ramen/ocio/manager.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{

ocio_view_combo_t::ocio_view_combo_t( QWidget *parent) : ocio_combo_t( parent)
{
	std::vector<std::string> views;
	int default_view;

	ocio::manager_t& manager( app().ocio_manager());
	manager.get_views( manager.default_display(), views, default_view);

	for( int i = 0; i < views.size(); ++i)
		addItem( QString::fromStdString( views[i]));

	current_view_ = views[default_view];
	setCurrentIndex( default_view);
	connect( this, SIGNAL( currentIndexChanged(int)), this, SLOT( combo_index_changed(int)));
}

void ocio_view_combo_t::set_view( const std::string& v)
{
	int index = index_for_string( v);
	RAMEN_ASSERT( index != -1);

	current_view_ = v;
	setCurrentIndex( index);
}

void ocio_view_combo_t::update_views( const std::string& display)
{
	blockSignals( true);
	clear();

	std::vector<std::string> views;
	int default_view, new_index = -1;

	ocio::manager_t& manager( app().ocio_manager());
	manager.get_views( display, views, default_view);

	for( int i = 0; i < views.size(); ++i)
	{
		QString view = QString::fromStdString( views[i]);
		addItem( view);

		if( views[i] == current_view_)
			new_index = i;
	}

	blockSignals( false);

	if( new_index == -1)
	{
		new_index = default_view;
		current_view_ = views[new_index];
	}

	setCurrentIndex( new_index);
}

void ocio_view_combo_t::combo_index_changed( int indx)
{
	current_view_ = currentText().toStdString();
	view_changed( current_view_);
}

} // ui
} // ramen
