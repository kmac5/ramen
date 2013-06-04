// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/util/string.hpp>

#include<boost/lexical_cast.hpp>

namespace ramen
{
namespace util
{

namespace
{

std::string get_number( const std::string& str)
{
    try
    {
        // this is a quick fix, if all str are nums
        int num = boost::lexical_cast<int>( str);
        return str;
    }
    catch( ...) {}

    std::string::size_type n = str.length() - 1;

    while( n != 0)
    {
        char c = str[n];

        if( !isdigit( c))
            break;

        --n;
    }

    return std::string( str, n + 1, str.length());
}

} // unnamed

bool is_string_valid_identifier( const std::string& str)
{
	// TODO: write this!
	
	if( str.empty())
		return false;

	return true;
}

void increment_string_number( std::string& str)
{
    std::string num_str = get_number( str);

    if( num_str.empty())
        str.append( "_2");
    else
    {
        std::string base( str, 0, str.length() - num_str.length());
        int num = boost::lexical_cast<int>( num_str);
        num_str = boost::lexical_cast<std::string>( ++num);
        str = base + num_str;
    }
}

} // util
} // ramen
