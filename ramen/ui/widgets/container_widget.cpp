// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/container_widget.hpp>

namespace ramen
{
namespace ui
{

container_widget_t::container_widget_t( QWidget *parent) : QStackedWidget( parent), contents_(0) {}

void container_widget_t::set_contents( QWidget *w)
{
    clear_contents();

    if( w)
    {
        addWidget( w);
		setCurrentIndex( 0);
		contents_ = w;
    }
}

void container_widget_t::clear_contents()
{
    if( contents_)
    {
		removeWidget( contents_);
		contents_ = 0;
    }
}

} // ui
} // ramen
