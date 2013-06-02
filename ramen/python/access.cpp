// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/python/access.hpp>

#include<ramen/params/param.hpp>
#include<ramen/params/param_set.hpp>

namespace ramen
{
namespace python
{

access::access() {}

boost::python::object access::get_param_value( const param_t& p) const
{
	return p.to_python( p.value());
}

boost::python::object access::get_param_value( const param_t& p, float frame) const
{
	return p.to_python( p.value_at_frame( frame));
}

boost::python::list access::get_param_ids( const param_set_t& p) const
{
	boost::python::list l;

	//for( std::map<std::string, param_t*>::const_iterator it( p.param_map_.begin()); it != p.param_map_.end(); ++it)
	//	l.append( it->first);

	return l;
}

} // namespace
} // namespace
