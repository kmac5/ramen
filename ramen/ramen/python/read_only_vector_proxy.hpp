// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_PYTHON_READ_ONLY_VEC_PROXY_HPP
#define RAMEN_PYTHON_READ_ONLY_VEC_PROXY_HPP

#include"Python.h"

#include<boost/python.hpp>

#include<vector>

namespace ramen
{
namespace python
{

template<class T>
class read_only_vector_proxy_t
{
public:

    read_only_vector_proxy_t( const std::vector<T>& vec) : vec_( vec) {}

    std::size_t len() const { return vec_.size();}

    T getitem( long index) const { return vec_[ convert_index( index)];}

    static void bind( const char *class_name)
    {
        boost::python::class_<read_only_vector_proxy_t<T> >( class_name, boost::python::no_init)
            .def( "__len__", &read_only_vector_proxy_t<T>::len)
            .def( "__getitem__", &read_only_vector_proxy_t<T>::getitem)
            ;
    }

private:

    std::size_t convert_index( long index) const
    {
        long s = len();

        if( index < 0 )
            index += s;

        if( index >= s || index < 0 )
        {
            PyErr_SetString( PyExc_IndexError, "Index out of range");
            boost::python::throw_error_already_set();
        }

        return index;
    }

    const std::vector<T>& vec_;
};

} // namespace
} // namespace

#endif
