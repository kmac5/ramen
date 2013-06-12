// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_EXCEPTIONS_HPP
#define	RAMEN_IMAGEIO_EXCEPTIONS_HPP

#include<ramen/config.hpp>

#include<string>
#include<exception>

namespace ramen
{
namespace imageio
{

class RAMEN_API exception : public std::exception
{
public:
	
	exception();
	explicit exception( const std::string& msg);
	virtual ~exception() throw() {}
	
	virtual const char *what() const throw ();
	
private:
	
	std::string what_;
};
	
struct RAMEN_API unknown_image_format : public exception
{
    unknown_image_format();
};

struct RAMEN_API unsupported_image : public exception
{
    unsupported_image();
	explicit unsupported_image( const std::string& msg);
};

} // imageio
} // ramen

#endif
