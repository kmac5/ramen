// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/export_selected_nodes.hpp>

#include<map>

#include<boost/foreach.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/serialization/yaml_oarchive.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{

void export_selected_nodes( const boost::filesystem::path& p)
{
    /*
	boost::filesystem::path comp_dir( app().document().composition().composition_dir());

    composition_t comp;

	if( !comp_dir.empty())
		comp.set_composition_dir( comp_dir);
	
    std::map<node_t*, node_t*> relation;

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
        if( n.selected())
        {
            std::auto_ptr<node_t> nclone( new_clone( n));
            relation[ &n] = nclone.get();
            comp.add_node( nclone);
        }
    }

    BOOST_FOREACH( edge_t& e, app().document().composition().edges())
    {
        if( e.src->selected() && e.dst->selected())
            comp.add_edge( edge_t( relation[e.src], relation[e.dst], e.port));
    }

	comp.set_composition_dir( p.parent_path());
	
	try
	{
		serialization::yaml_oarchive_t out;
		out.write_composition_header();
		comp.write( out);
		out.write_to_file( p);		
	}
	catch( std::exception& e)
	{
		app().ui()->error( std::string( "Couldn't save file. Exception, what = ") + e.what());
	}
    */
}

} // namespace
