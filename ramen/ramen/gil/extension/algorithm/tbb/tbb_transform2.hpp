//  Copyright Esteban Tovagliari 2008. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef GIL_EXTENSION_TBB_ALGO_PARALLEL_TRANSFORM2_HPP
#define GIL_EXTENSION_TBB_ALGO_PARALLEL_TRANSFORM2_HPP

#include"tbb_config.hpp"

#include<boost/gil/algorithm.hpp>

namespace boost
{
namespace gil
{
namespace detail
{

template<class View1, class View2, class View3, class Fun>
struct tbb_transform2_pixels_fun
{
    tbb_transform2_pixels_fun( const View1& src1, const View2& src2,
                                const View3& dst, Fun f) : src1_(src1), src2_(src2), dst_(dst), f_(f) {}

    void operator()( const tbb::blocked_range<size_t>& r) const
    {
        for( std::size_t i = r.begin(); i != r.end(); ++i)
        {
            typename View1::x_iterator srcIt1=src1_.row_begin(i);
            typename View2::x_iterator srcIt2=src2_.row_begin(i);
            typename View3::x_iterator dstIt=dst_.row_begin(i);

            for( int x = 0, e = dst_.width(); x < e; ++x)
                dstIt[x]=f_( srcIt1[x], srcIt2[x]);
        }
    }

    const View1& src1_;
    const View2& src2_;
    const View3& dst_;
    Fun f_;
};

template<class View1, class View2, class View3, class Fun>
tbb_transform2_pixels_fun<View1, View2, View3, Fun>
make_tbb_transform2_pixels_fun( const View1& src1, const View2& src2, const View3& dst, Fun f)
{
    return tbb_transform2_pixels_fun<View1, View2, View3, Fun>( src1, src2, dst, f);
}

} // detail

template<class View1, class View2, class View3, class Fun>
GIL_FORCEINLINE void tbb_transform2_pixels( const View1& src1, const View2& src2, const View3& dst, Fun f)
{
    tbb::parallel_for( tbb::blocked_range<std::size_t>( 0, src1.height()),
                        detail::make_tbb_transform2_pixels_fun( src1, src2, dst, f),
                        GIL_TBB_ALGORITHMS_DEFAULT_PARTITIONER());
}

} // namespace
} // namespace

#endif
