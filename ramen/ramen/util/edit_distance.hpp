// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UTIL_EDIT_DISTANCE_HPP
#define	RAMEN_UTIL_EDIT_DISTANCE_HPP

#include<string>
#include<vector>

#include<boost/scoped_array.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace util
{

struct edit_distance_t
{
public:

	edit_distance_t();

	int operator()( const std::string& src, const std::string& dst);
	
private:

	void realloc_matrix( int rows, int cols);
	
	int matrix( int i, int j) const
	{
		RAMEN_ASSERT( i >= 0 && i < rows_);
		RAMEN_ASSERT( j >= 0 && j < cols_);
		return matrix_[i * cols_ + j];
	}

	int& matrix( int i, int j)
	{ 
		RAMEN_ASSERT( i >= 0 && i < rows_);
		RAMEN_ASSERT( j >= 0 && j < cols_);
		return matrix_[i * cols_ + j];
	}
	
	boost::scoped_array<int> matrix_;
	int rows_, cols_;
};
	
} // util
} // ramen

#endif
