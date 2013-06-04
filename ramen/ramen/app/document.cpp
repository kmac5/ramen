// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/document.hpp>

#include<boost/bind.hpp>
#include<boost/filesystem/fstream.hpp>

#include<ramen/app/application.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>

namespace ramen
{

document_t::document_t() : dirty_( false)
{
    undo_.reset( new undo::stack_t());
}

document_t::~document_t() {}

void document_t::set_file( const boost::filesystem::path& p)
{
	RAMEN_ASSERT( p.empty() || p.is_absolute());

    file_ = p;
	composition().set_composition_dir( file_.parent_path());
}

void document_t::load( serialization::yaml_iarchive_t& in)
{
	RAMEN_ASSERT( has_file());
	composition().read( in);
}

void document_t::save( serialization::yaml_oarchive_t& out) const
{
	RAMEN_ASSERT( out.header_written());
	RAMEN_ASSERT( out.map_level() == 1);

	composition().write( out);
}

} // namespace
