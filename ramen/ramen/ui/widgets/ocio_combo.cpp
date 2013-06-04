// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/ocio_combo.hpp>

namespace ramen
{
namespace ui
{

ocio_combo_t::ocio_combo_t( QWidget *parent) : QComboBox( parent)
{
	setFocusPolicy( Qt::NoFocus);
}

int ocio_combo_t::index_for_string( const std::string& s) const
{
	return findText( QString::fromStdString( s));
}

} // ui
} // ramen
