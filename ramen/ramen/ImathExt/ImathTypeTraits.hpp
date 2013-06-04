//  (C) Copyright Esteban Tovagliari 2011.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMATHEXT_IMATH_TYPE_TRAITS_HPP
#define RAMEN_IMATHEXT_IMATH_TYPE_TRAITS_HPP

#include<boost/type_traits/has_nothrow_constructor.hpp>
#include<boost/type_traits/has_nothrow_copy.hpp>
#include<boost/type_traits/has_nothrow_assign.hpp>
#include<boost/type_traits/has_nothrow_destructor.hpp>

#include<OpenEXR/ImathColor.h>
#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImathMatrix.h>

#define RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Type) \
template<class T> struct has_nothrow_constructor<Imath::Type<T> > : public true_type {}; \
template<class T> struct has_nothrow_copy<Imath::Type<T> > : public true_type {}; \
template<class T> struct has_nothrow_assign<Imath::Type<T> > : public true_type {}; \
template<class T> struct has_nothrow_destructor<Imath::Type<T> > : public true_type {};

namespace boost
{

RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Vec2)
RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Vec3)
RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Vec4)

RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Color3)
RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Color4)

RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Box)

RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Matrix33)
RAMEN_IMATH_SPECIALIZE_TYPE_TRAITS( Matrix44)

} // boost

#endif
