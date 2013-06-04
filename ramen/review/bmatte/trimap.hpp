/*
 *  trimap.hpp
 *  trimap
 *
 */

#include<cstdio>

#include<vector>
#include<algorithm>
#include<utility>

#include<boost/cstdint.hpp>
using boost::uint8_t;

#include<blitz/array.h>

enum
{
	bg_val=0,
	uk_val=128,
	fg_val=255,
	
	mask_val = 77 // any value
};

class trimap
{
public:

	typedef std::vector<std::pair<int,int> >::const_iterator contour_iterator;

	trimap( const blitz::Array<uint8_t,2>& trimap) : data_( trimap.copy()), numfg_(0), numbg_(0), numuk_(0)
	{
	}
	
	std::size_t width() const	{ return data_.cols();}
	std::size_t height() const	{ return data_.rows();}
	
	uint8_t operator()( int j, int i) const { return data_( j, i);}

	void thereshold( uint8_t low, uint8_t high)
	{
	uint8_t p;
	
		for( int j=0; j < data_.rows(); ++j)
		{
			for( int i=0; i < data_.cols(); ++i)
			{
				p = data_( j, i);

				if( p < low)
				{
					data_( j, i) = bg_val;
					++numbg_;
				}
				else
				{
					if( p > high)
					{
						data_( j, i) = fg_val;
						++numfg_;
					}
					else
					{
						data_( j, i) = uk_val;
						++numuk_;
					}
				}
			}
		}
	}

	bool grow()
	{
		contour_.clear();
		
		for( int j=1; j < data_.rows()-1; ++j)
		{
			for( int i=1; i < data_.cols()-1; ++i)
			{
				if( data_(j, i) != uk_val)
				{
					if( data_(j-1, i) == uk_val || data_(j+1, i) == uk_val || data_(j, i-1) == uk_val || data_(j, i+1) == uk_val)
						contour_.push_back( std::make_pair( j, i));
				}
			}
		}

		if( contour_.empty())
			return false;

		contour_iterator it( contour_.begin());
	
		for( ; it < contour_.end(); ++it)
			data_( it->first, it->second) = uk_val;
		
		return true;
	}

	bool shrink()
	{
		contour_.clear();
				
		for( int j=1; j < data_.rows()-1; ++j)
		{
			for( int i=1; i < data_.cols()-1; ++i)
			{
				if( data_(j, i) == uk_val)
				{
					if( (data_(j-1, i) != uk_val) || (data_(j+1, i) != uk_val) || (data_(j, i-1) != uk_val) || (data_(j, i+1) != uk_val))
						contour_.push_back( std::make_pair( j, i));
				}
			}
		}

		if( contour_.empty())
			return false;

		contour_iterator it( contour_.begin());
	
		for( ; it < contour_.end(); ++it)
			data_( it->first, it->second) = mask_val;
		
		return true;
	}

	contour_iterator contour_begin()	{ return contour_.begin();}
	contour_iterator contour_end()		{ return contour_.end();}

	std::size_t num_fg()	{ return numfg_;}
	std::size_t num_bg()	{ return numbg_;}
	std::size_t num_uk()	{ return numuk_;}

	uint8_t *ptr()	{ return data_.dataFirst();}

public:

	blitz::Array<uint8_t,2> data_;
	std::vector< std::pair< int, int> > contour_;
	
	std::size_t numfg_,numbg_,numuk_;
};
