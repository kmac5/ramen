// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_UTIL_CALCULATOR_HPP
#define RAMEN_UTIL_CALCULATOR_HPP

#include<string>

#include<boost/noncopyable.hpp>
#include<boost/optional.hpp>

namespace ramen
{
namespace util
{

class calculator_t : boost::noncopyable
{
public:

	calculator_t();
	~calculator_t();

	boost::optional<double> operator()( const std::string& str) const;
	
private:

	struct impl;
	impl *pimpl_;
};
	
} // util
} // ramen

#endif
