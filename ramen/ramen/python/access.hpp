// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_PYTHON_ACCESS_HPP
#define RAMEN_PYTHON_ACCESS_HPP

#include<ramen/python/access_fwd.hpp>

#include<ramen/python/python.hpp>

#include<boost/python/object.hpp>
#include<boost/python/list.hpp>

#include<ramen/params/param_fwd.hpp>
#include<ramen/params/param_set_fwd.hpp>

namespace ramen
{
namespace python
{

class access
{
public:

	access();

	boost::python::object get_param_value( const param_t& p) const;
	boost::python::object get_param_value( const param_t& p, float frame) const;

	boost::python::list get_param_ids( const param_set_t& p) const;

private:

};
	
} // namespace
} // namespace

#endif
