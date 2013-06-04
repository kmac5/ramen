// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/movieio/exceptions.hpp>

#include<boost/lexical_cast.hpp>

namespace ramen
{
namespace movieio
{

exception::exception() {}	
exception::exception( const std::string& msg) : what_( msg) {}
		
const char *exception::what() const throw()
{
	if( !what_.empty())
		return what_.c_str();
		
	return "";
}
	
unknown_movie_format::unknown_movie_format() : exception( "Unknown file format") {}

unsupported_movie::unsupported_movie() : exception( "Unsupported file type") {}
unsupported_movie::unsupported_movie( const std::string& msg) : exception( msg) {}

frame_out_of_bounds::frame_out_of_bounds( int num) : exception()
{
	what_ = std::string( "Frame out of bounds: ") + boost::lexical_cast<std::string>( num);
}

} // namespace
} // namespace
