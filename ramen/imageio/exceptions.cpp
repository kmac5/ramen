// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/exceptions.hpp>

namespace ramen
{
namespace imageio
{

exception::exception() {}	
exception::exception( const std::string& msg) : what_( msg) {}
	
const char *exception::what() const throw()
{
	if( !what_.empty())
		return what_.c_str();
	
	return "";
}
	
unknown_image_format::unknown_image_format() : exception( "Unknown image format") {}

unsupported_image::unsupported_image() : exception( "Unsupported image type") {}
unsupported_image::unsupported_image( const std::string& msg) : exception( msg) {}

} // namespace
} // namespace
