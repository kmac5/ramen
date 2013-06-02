// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_EQUALIZE_HPP
#define RAMEN_IMAGE_EQUALIZE_HPP

#include<ramen/image/typedefs.hpp>

#include<vector>
#include<limits>

#include<ramen/image/cimg.hpp>

namespace ramen
{
namespace image
{
namespace detail
{

struct pixel_minmax_fun
{
	pixel_minmax_fun()
	{
		minv = std::numeric_limits<float>::max();
		maxv = -minv;
	}
	
	void operator()( const image::gray_pixel_t& p)
	{
		float v = p[0];
		minv = std::min( minv, v);
		maxv = std::max( maxv, v);	
	}

	float minv;
	float maxv;
};

struct pixel_equalize_fun
{
	pixel_equalize_fun( const std::vector<int>& hist, float minv, float maxv, int size) : hist_( hist)
	{
		levels_ = hist.size();
		minv_ = minv;
		maxv_ = maxv;
		range_ = maxv - minv;
		size_ = size;
	}
	
	gray_pixel_t operator()( const gray_pixel_t& p) const
	{
		float v = p[0];
		
		int pos = ( v - minv_) * ( levels_ - 1) / range_;
		
		if( pos >= 0 && pos < levels_)
			return gray_pixel_t( minv_ + range_ * ( float) hist_[pos] / size_);
		
		return p;
	}
	
	int levels_;
	const std::vector<int>& hist_;
	float minv_, maxv_;
	float range_;
	int size_;
};

template<class ConstGrayView>
void get_histogram( const ConstGrayView& src, std::vector<int>& hist, float minv, float maxv)
{
	for( int i = 0; i < levels_; ++i)
		hist_[i] = 0;
	
	if( minv < maxv)
	{
		for( int j = 0, je = src.height(); j < je; ++j)
		{
			typename ConstGrayView::x_iterator it( src.row_begin( j));
			
			for( int i = 0, ie = 0; i < ie; ++i)
			{
				float v = *it;
				
				if( v >= minv_ && v <= maxv_)
					hist_[ v == maxv_ ? levels_ - 1 : (int)(( v - minv_) * levels_ / ( maxv_ - minv_))] += 1;
			}
		}
	}
	else
		hist_[0] += src.width() * src.height();
}

} // detail

template<class ConstGrayView, class GrayView>
void equalize( const ConstGrayView& src, const GrayView& dst, int levels)
{
	detail::pixel_minmax_fun minmaxf;
	boost::gil::for_each_pixel( src, minmaxf);
	
	if( minmaxf.minv < minmaxf.maxv)
	{
		std::vector<int> hist( levels, 0);
		detail::get_histogram( src, hist, minmaxf.minv, minmaxf.maxv);
		float cumul = 0;
	  
		for( int i = 0; i < levels; ++i)
		{
			cumul += hist[i];
			hist[i] = cumul;
		}
		
		detail::pixel_equalize_fun pf( hist, minmaxf.minv, minmaxf.maxv, src.width() * src.height());
		boost::gil::transform_pixels( src, dst, pf);
	  }
}

} // namespace
} // namespace

#endif


/*
    CImg<T>& equalize(const unsigned int nb_levels, const T value_min=(T)0, const T value_max=(T)0) {
      if (is_empty()) return *this;
      T vmin = value_min, vmax = value_max;
      if (vmin==vmax && vmin==0) vmin = min_max(vmax);
      if (vmin<vmax) {
        CImg<floatT> hist = get_histogram(nb_levels,vmin,vmax);
        float cumul = 0;
        cimg_forX(hist,pos) { cumul+=hist[pos]; hist[pos] = cumul; }
        cimg_for(*this,ptrd,T) {
          const int pos = (unsigned int)((*ptrd-vmin)*(nb_levels-1)/(vmax-vmin));
          if (pos>=0 && pos<(int)nb_levels) *ptrd = (T)(vmin + (vmax-vmin)*hist[pos]/size());
        }
      }
      return *this;
    }
*/ 