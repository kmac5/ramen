// Copyright (c) 2012 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_PARAMS_COMPOSITE_PARAMETERISED_HPP
#define	RAMEN_PARAMS_COMPOSITE_PARAMETERISED_HPP

#include<ramen/params/parameterised.hpp>

#include<boost/ptr_container/ptr_vector.hpp>

namespace ramen
{

/**
\ingroup params
\brief Base class for parameterised objects that can contain children parameterised objects.
*/
class RAMEN_API composite_parameterised_t : public parameterised_t
{
public:

    typedef boost::ptr_vector<parameterised_t> parameterised_container_type;
    typedef parameterised_container_type::const_iterator    const_iterator;
    typedef parameterised_container_type::iterator          iterator;

    composite_parameterised_t();
	virtual ~composite_parameterised_t();

protected:

	composite_parameterised_t( const composite_parameterised_t& other);
	void operator=( const parameterised_t& other);

    void add_parameterised( std::auto_ptr<parameterised_t> p);
    std::auto_ptr<parameterised_t> remove_parameterised( parameterised_t *p);

    const parameterised_container_type& children() const    { return children_;}
    parameterised_container_type& children()                { return children_;}

    const_iterator begin() const { return children_.begin();}
    const_iterator end() const   { return children_.end();}

    iterator begin() { return children_.begin();}
    iterator end()   { return children_.end();}

private:

    parameterised_container_type children_;
};

} // namespace

#endif
