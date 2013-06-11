// Copyright (c) 2009 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMPORT_COMPOSITION_HPP
#define	RAMEN_IMPORT_COMPOSITION_HPP

#include<utility>

#include<ramen/filesystem/path.hpp>

#include<ramen/serialization/yaml_iarchive.hpp>

namespace ramen
{

std::auto_ptr<serialization::yaml_iarchive_t> import_composition( const boost::filesystem::path& p);

} // ramen

#endif


