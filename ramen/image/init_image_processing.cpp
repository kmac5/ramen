// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/init_image_processing.hpp>

#include<ramen/system/system.hpp>
#include<ramen/image/processing.hpp>

#include<ramen/image/filters.hpp>

namespace ramen
{
namespace image
{

void init_image_processing()
{
	// init filter tables
	lanczos3_filter_t::init_table();
	mitchell_filter_t::init_table();
	catrom_filter_t::init_table();
}

} // image
} // ramen
