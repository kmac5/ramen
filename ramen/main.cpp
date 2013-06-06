// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<stdlib.h>
#include<string.h>

#include<iostream>
#include<exception>

#include<boost/version.hpp>
#include<boost/static_assert.hpp>

#include<ramen/app/application.hpp>

#include<ramen/filesystem/path.hpp>

void ramen_terminate( void)
{
	ramen::app().fatal_error( "Ramen has encountered a fatal error");
}
	
void ramen_unexpected( void)
{
	ramen::app().error( "Ramen has encountered an unexpected exception");
	throw std::runtime_error( "Ramen has encountered an unexpected exception.\nPlease report this bug." );
}

int main( int argc, char **argv)
{
	std::set_terminate( &ramen_terminate);
	std::set_unexpected( &ramen_unexpected);
	
	try
	{
        ramen::application_t r_app( argc, argv);
		int result = r_app.run();
		return result;
	}
	catch( std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
    }
	catch( ...)
	{
		std::cerr << "Unknown exception" << std::endl;
    }
}
