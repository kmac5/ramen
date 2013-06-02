// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/app/import_composition.hpp>

#include<boost/foreach.hpp>
#include<boost/filesystem/fstream.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

namespace ramen
{

std::auto_ptr<serialization::yaml_iarchive_t> import_composition( const boost::filesystem::path& p)
{
	composition_t comp;
	comp.set_composition_dir( p.parent_path());
	
    boost::filesystem::ifstream ifs( p, serialization::yaml_iarchive_t::file_open_mode());

    if( !ifs.is_open() || !ifs.good())
		throw std::runtime_error( "Couldn't open file");

	std::auto_ptr<serialization::yaml_iarchive_t> in;
	
    /*
	in.reset( new serialization::yaml_iarchive_t( ifs));
		
	if( !in->read_composition_header())
		throw std::runtime_error( "Bad file header");

	comp.read( *in);

	app().document().composition().deselect_all();
	comp.select_all();

	app().document().composition().merge_composition( comp);
    */
	return in;
}

} // namespace
