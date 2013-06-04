// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/rdk/rdk.hpp>

#include<ramen/version.hpp>

#include<ramen/nodes/node_factory.hpp>

void rdk_register_node( const ramen::node_metaclass_t& m)
{
    ramen::node_factory_t::instance().register_node( m);
}

rdk_app_info_t::rdk_app_info_t()
{
	host = "Ramen";
	major_version = RAMEN_VERSION_MAJOR;
	minor_version = RAMEN_VERSION_MINOR;
	rdk_version = 1;
	register_node = &rdk_register_node;
}
