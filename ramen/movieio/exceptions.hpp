// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_MOVIEIO_EXCEPTIONS_HPP
#define	RAMEN_MOVIEIO_EXCEPTIONS_HPP

#include<ramen/config.hpp>

#include<exception>
#include<string>

namespace ramen
{
namespace movieio
{

class RAMEN_API exception : public std::exception
{
public:
	
	exception();
	explicit exception( const std::string& msg);
	virtual ~exception() throw() {}
		
	virtual const char *what() const throw ();
		
protected:
		
	std::string what_;
};
	
struct RAMEN_API unknown_movie_format : public exception
{
    unknown_movie_format();
};

struct RAMEN_API unsupported_movie : public exception
{
    unsupported_movie();
	explicit unsupported_movie( const std::string& msg);	
};

struct RAMEN_API frame_out_of_bounds : public exception
{
	explicit frame_out_of_bounds( int num);
};

} // movieio
} // ramen

#endif
