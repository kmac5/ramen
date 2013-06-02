//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_TBB_ALGO_PARALLEL_TRANSFORM_HPP
#define GIL_EXTENSION_TBB_ALGO_PARALLEL_TRANSFORM_HPP

#include"tbb_config.hpp"

#include<boost/gil/algorithm.hpp>

namespace boost
{
namespace gil
{
namespace detail
{

template<class View1, class View2, class Fun>
struct tbb_transform_pixels_fun
{
    tbb_transform_pixels_fun( const View1& src, const View2& dst, Fun f) : src_(src), dst_(dst), f_(f) {}
	
    void operator()( const tbb::blocked_range<size_t>& r) const
    {
        for( std::size_t i = r.begin(), e = r.end(); i != e; ++i)
            std::transform( src_.row_begin(i), src_.row_end(i), dst_.row_begin(i), f_);
    }
	
    const View1& src_;
    const View2& dst_;
    Fun f_;
};

template<class View1, class View2, class Fun>
tbb_transform_pixels_fun<View1, View2, Fun> make_tbb_transform_pixels_fun( const View1& src, const View2& dst, Fun f)
{
    return tbb_transform_pixels_fun<View1, View2, Fun>( src, dst, f);
}

} // detail

template<class View1, class View2, class Fun>
GIL_FORCEINLINE void tbb_transform_pixels( const View1& src, const View2& dst, Fun f)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src.height()),
                        detail::make_tbb_transform_pixels_fun( src, dst, f),
                            GIL_TBB_ALGORITHMS_DEFAULT_PARTITIONER());
}

} // namespace
} // namespace

#endif
