// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/flipbook/djv_flipbook.hpp>

#include<ramen/flipbook/factory.hpp>

namespace ramen
{
namespace flipbook
{

djv_flipbook_t::djv_flipbook_t( int frame_rate, 
								const std::string& display_device,
								const std::string& display_transform) : external_flipbook_t( frame_rate, 
																							display_device,
																							display_transform)
{
}

djv_flipbook_t *djv_flipbook_t::create( int frame_rate, const std::string& display_device, const std::string& display_transform)
{
	return new djv_flipbook_t( frame_rate, display_device, display_transform);
}

std::string djv_flipbook_t::program() const { return "djv_view";}

void djv_flipbook_t::arguments( std::vector<std::string>& args) const
{
	args.push_back( "-playback_speed");
	args.push_back( boost::lexical_cast<std::string>( frame_rate_));
	
	std::string seq = filesystem::file_string( dir_);
	seq += "/";
	seq += fname_;
	seq += "#";
	seq += ext_;
	args.push_back( seq);
}

static bool registered = factory_t::instance().register_flipbook( "djv", &djv_flipbook_t::create);

} // namespace
} // namespace
