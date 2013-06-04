// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_RDK_HPP
#define RAMEN_RDK_HPP

#include<ramen/config.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/nodes/node_metaclass.hpp>

enum rdk_error_t
{
	rdk_no_err = 0,
	rdk_unknown_err
};

// struct passed to the plugins entry point
struct RAMEN_API rdk_app_info_t
{
	rdk_app_info_t();

	const char *host;
	int major_version;
	int minor_version;
	int rdk_version;

	void (*register_node)( const ramen::node_metaclass_t& m);
};

#endif
