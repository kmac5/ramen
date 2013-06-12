// Copyright (c) 2013 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_CORE_EXCEPTIONS_HPP
#define RAMEN_CORE_EXCEPTIONS_HPP

#include<ramen/config.hpp>

#include<typeinfo>

#include<ramen/core/name.hpp>
#include<ramen/core/string.hpp>
#include<ramen/core/types.hpp>

namespace ramen
{
namespace core
{

/*!
\ingroup core
\brief Exception class.
*/
class RAMEN_API exception
{
public:

    exception() {}
    virtual ~exception() {}

    virtual const char *what() const = 0;
};

/*!
\ingroup core
\brief Exception class.
*/
class RAMEN_API runtime_error : public exception
{
public:

    explicit runtime_error( string_t message);

    virtual const char *what() const;

private:

    string_t message_;
};

/*!
\ingroup core
\brief Exception class.
*/
class RAMEN_API bad_cast : public exception
{
public:

    bad_cast( const std::type_info& src_type, const std::type_info& dst_type);

    virtual const char *what() const;

private:

    string_t message_;
};

/*!
\ingroup core
\brief Exception class.
*/
class RAMEN_API bad_type_cast : public exception
{
public:

    bad_type_cast( const type_t& src_type, const type_t& dst_type);

    virtual const char *what() const;

private:

    core::string_t message_;
};

/*!
\ingroup core
\brief Exception class.
*/
class RAMEN_API key_not_found : public exception
{
public:

    explicit key_not_found( const name_t& name);
    explicit key_not_found( string_t message);

    virtual const char *what() const;

private:

    string_t message_;
};

/*!
\ingroup core
\brief Exception class.
*/
class RAMEN_API not_implemented
{
public:

    not_implemented();
};

} // core
} // ramen

#endif
