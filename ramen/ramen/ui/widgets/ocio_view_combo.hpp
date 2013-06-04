// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_QR_OCIO_VIEW_COMBO_HPP
#define RAMEN_QR_OCIO_VIEW_COMBO_HPP

#include<ramen/ui/widgets/ocio_combo.hpp>

namespace ramen
{
namespace ui
{

class ocio_view_combo_t : public ocio_combo_t
{
	Q_OBJECT

public:

	ocio_view_combo_t( QWidget *parent = 0);

	const std::string& get_current_view() const { return current_view_;}

public Q_SLOTS:

	void set_view( const std::string& s);

	void update_views( const std::string& display);

Q_SIGNALS:

    void view_changed( const std::string&);

private Q_SLOTS:

	void combo_index_changed( int indx);

private:

	std::string current_view_;
};

} // ui
} // ramen

#endif
