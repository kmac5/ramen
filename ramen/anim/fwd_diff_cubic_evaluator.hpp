// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ANIM_FWD_DIFF_CUBIC_EVALUATOR_HPP
#define	RAMEN_ANIM_FWD_DIFF_CUBIC_EVALUATOR_HPP

#include<boost/array.hpp>

namespace ramen
{
namespace anim
{

template<class T=float>
class fwd_diff_cubic_evaluator_t
{
public:

	typedef T value_type;

    fwd_diff_cubic_evaluator_t( const boost::array<value_type,4>& p, value_type step_size)
    {
        value_type s = step_size;
        value_type s2 = s * s;
        value_type s3 = s2 * s;

        value_ = p[3];
        diff1_ = p[0] * s3 + p[1] * s2 + p[2] * s;
        diff2_ = value_type(6) * p[0] * s3 + value_type(2) * p[1] * s2;
        diff3_ = value_type(6) * p[0] * s3;
    }

    value_type operator()() const { return value_;}

    fwd_diff_cubic_evaluator_t& operator++()
    {
        value_ += diff1_;
        diff1_ += diff2_;
        diff2_ += diff3_;
        return *this;
    }

    fwd_diff_cubic_evaluator_t operator++( int)
    {
        fwd_diff_cubic_evaluator_t tmp( *this);
        ++( *this);
        return tmp;
    }

private:

    value_type value_;
    value_type diff1_, diff2_, diff3_;
};

} // anim
} // ramen

#endif
