// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/util/command_line_parser.hpp>

#include<stdlib.h>
#include<string.h>

#include<ramen/assert.hpp>

namespace ramen
{
namespace util
{

command_line_parser_t::command_line_parser_t( int cmd_argc, char **cmd_argv) : argv( 0)
{
    RAMEN_ASSERT( cmd_argc >= 1);

    argc = cmd_argc;
    argv = reinterpret_cast<char**>( malloc( argc * sizeof( char*)));

    for( int i = 0; i < argc ; ++i)
        argv[i] = strdup( cmd_argv[i]);
}

command_line_parser_t::~command_line_parser_t()
{
    for( int i = 0; i < argc ; ++i)
        free( reinterpret_cast<void*>( argv[i]));

    free( reinterpret_cast<void*>( argv));
}

} // util
} // ramen
