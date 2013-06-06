// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.



#include<ramen/ui/compview/composition_view.hpp>

#include<QHBoxLayout>

namespace ramen
{
namespace ui
{

composition_view_toolbar_t::composition_view_toolbar_t( composition_view_t *comp_view) : QWidget()
{
    comp_view_ = comp_view;

    setMinimumSize( 0, toolbar_height());
    setMaximumSize( QWIDGETSIZE_MAX, toolbar_height());
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);

	/*
    QHBoxLayout *horizontalLayout = new QHBoxLayout( this);
    horizontalLayout->setContentsMargins( 0, 0, 0, 0);
	
	QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem( horizontalSpacer);

    setLayout( horizontalLayout);
	*/
}

int composition_view_toolbar_t::toolbar_height() const { return 30;}

} // namespace
} // namespace
