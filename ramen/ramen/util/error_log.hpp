// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UTIL_ERROR_LOG_HPP
#define RAMEN_UTIL_ERROR_LOG_HPP

#include<sstream>

namespace ramen
{
namespace util
{

class error_log_t
{
public:

	error_log_t();
	
	template<class T>
	error_log_t& operator<<( const T& x)
	{
		error_stream() << x;
		return *this;
	}
	
	std::stringstream& error_stream();
	std::string errors() const;
	
private:
	
	std::stringstream error_stream_;
};

} // util
} // ramen

#endif
