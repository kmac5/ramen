// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/util/error_log.hpp>

namespace ramen
{
namespace util
{

error_log_t::error_log_t() {}

std::stringstream& error_log_t::error_stream() { return error_stream_;}

std::string error_log_t::errors() const
{
	return error_stream_.str();
}

} // util
} // ramen
