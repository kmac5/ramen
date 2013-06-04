// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_WIDGETS_OCIO_COLORSPACE_COMBO_HPP
#define RAMEN_UI_WIDGETS_OCIO_COLORSPACE_COMBO_HPP

#include<ramen/ui/widgets/ocio_colorspace_combo_fwd.hpp>

#include<ramen/ui/widgets/ocio_combo.hpp>

namespace ramen
{
namespace ui
{

class ocio_colorspace_combo_t : public ocio_combo_t
{
	Q_OBJECT

public:

	ocio_colorspace_combo_t( QWidget *parent = 0);

	const std::string& get_current_colorspace() const { return current_colorspace_;}

    bool set_colorspace_or_default( const std::string& s);

public Q_SLOTS:

	void set_colorspace( const std::string& s);

Q_SIGNALS:

    void colorspace_changed( const std::string&);

private Q_SLOTS:

	void combo_index_changed( int indx);

private:

	void set_default();

	std::string current_colorspace_;
};

} // ui
} // ramen

#endif
