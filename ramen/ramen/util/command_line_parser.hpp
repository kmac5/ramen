// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UTIL_COMMAND_LINE_PARSER_HPP
#define RAMEN_UTIL_COMMAND_LINE_PARSER_HPP

#include<ramen/util/command_line_parser_fwd.hpp>

namespace ramen
{
namespace util
{

class command_line_parser_t
{
public:

    command_line_parser_t( int cmd_argc, char **cmd_argv);
    ~command_line_parser_t();

    int argc;
    char **argv;

private:

};

} // util
} // ramen

#endif
