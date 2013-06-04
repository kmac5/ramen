/*
 * optical flow.
 */

#include<cassert>
#include<cmath>
#include<algorithm>
#include<vector>
#include<utility>
#include<limits>
#include<iostream>

#include<boost/bind.hpp>

#include<tbb/task_scheduler_init.h>
#include<tbb/blocked_range.h>
#include<tbb/parallel_for.h>

#include<CImg/CImg.h>
using namespace cimg_library;

#include<OpenEXR/ImathBox.h>

// util
template<class T>
inline T clamp( T x, T lo, T hi)
{
    if( x < lo)
        return lo;

    if( x > hi)
        return hi;

    return x;
}

template<class T>
inline T square( T x) { return x * x;}

template<class T>
inline T abs( T x)
{
    if( x < 0)
        return -x;

    return x;
}

template<class T>
inline T sign( T x)
{
    if( x < 0)
        return T(-1);

    return T(1);
}

template<class T>
Imath::Box<Imath::Vec2<T> > intersect( const Imath::Box<Imath::Vec2<T> >& a, const Imath::Box<Imath::Vec2<T> >& b)
{
    if( !( a.min.x > b.max.x || a.max.x < b.min.x || a.min.y > b.max.y || a.max.y < b.min.y))
    {
        return Imath::Box<Imath::Vec2<T> >( Imath::Vec2<T>( std::max( a.min.x, b.min.x), std::max( a.min.y, b.min.y)),
			                              Imath::Vec2<T>( std::min( a.max.x, b.max.x), std::min( a.max.y, b.max.y)));
    }
    else
        return Imath::Box<Imath::Vec2<T> >();
}

template<class T>
Imath::Box<T> offset_by( const Imath::Box<T>& box, const T& d)
{
    Imath::Box<T> newbox( box);
    newbox.min += d;
    newbox.max += d;
    return newbox;
}

// misc utils
void convert_to_bw( const CImg<>& src, CImg<>& dst)
{
    for( int j = 0; j < src.height(); ++j)
    {
	for( int i = 0; i < src.width(); ++i)
	{
            dst( i, j) = (( 0.3f * src( i, j, 0, 0)) +
                         ( 0.59f * src( i, j, 0, 1)) +
                         ( 0.11f * src( i, j, 0, 2))) / 255.0f;
	}
    }
}

// util
int ncols = 0;
#define MAXCOLS 60
int colorwheel[MAXCOLS][3];

void setcols(int r, int g, int b, int k)
{
    colorwheel[k][0] = r;
    colorwheel[k][1] = g;
    colorwheel[k][2] = b;
}

void makecolorwheel()
{
    // relative lengths of color transitions:
    // these are chosen based on perceptual similarity
    // (e.g. one can distinguish more shades between red and yellow
    //  than between yellow and green)
    int RY = 15;
    int YG = 6;
    int GC = 4;
    int CB = 11;
    int BM = 13;
    int MR = 6;
    ncols = RY + YG + GC + CB + BM + MR;

    if (ncols > MAXCOLS)
	exit(1);

    int i;
    int k = 0;
    for (i = 0; i < RY; i++) setcols(255,	   255*i/RY,	 0,	       k++);
    for (i = 0; i < YG; i++) setcols(255-255*i/YG, 255,		 0,	       k++);
    for (i = 0; i < GC; i++) setcols(0,		   255,		 255*i/GC,     k++);
    for (i = 0; i < CB; i++) setcols(0,		   255-255*i/CB, 255,	       k++);
    for (i = 0; i < BM; i++) setcols(255*i/BM,	   0,		 255,	       k++);
    for (i = 0; i < MR; i++) setcols(255,	   0,		 255-255*i/MR, k++);
}

void compute_color(float fx, float fy, unsigned char *pix)
{
    if(ncols == 0)
        makecolorwheel();

    float rad = std::sqrt( fx * fx + fy * fy);
    float a = std::atan2( -fy, -fx) / M_PI;
    float fk = ( a + 1.0) / 2.0f * ( ncols - 1);
    int k0 = (int) fk;
    int k1 = ( k0 + 1) % ncols;
    float f = fk - k0;

    for(int b = 0; b < 3; b++)
    {
        float col0 = colorwheel[k0][b] / 255.0;
        float col1 = colorwheel[k1][b] / 255.0;
        float col = (1 - f) * col0 + f * col1;

        if(rad <= 1)
            col = 1 - rad * (1 - col); // increase saturation with radius
        else
            col *= .75; // out of range

        pix[2 - b] = ( int) ( 255.0 * col);
    }
}

void animate_warp(const CImg<unsigned char>& src, const CImg<unsigned char>& dest, const CImg<>& U, int nb, CImgDisplay& disp)
{
    CImg<unsigned char> visu( src);
    CImg<unsigned char> warp( src);
    float t = 0;

    for(unsigned int iteration = 0; !disp || (!disp.is_closed() && !disp.is_keyQ()); ++iteration)
    {

        cimg_forXYC(warp, x, y, k)
        {
            float dx = U(x, y, 0);
            float dy = U(x, y, 1);
            float I1 = (float) src.linear_atXY(x - t * dx, y - t * dy, k);
            float I2 = (float) dest.linear_atXY(x + (1 - t) * dx, y + (1 - t) * dy, k);
            warp(x, y, k) = (unsigned char) ((1 - t) * I1 + t * I2);
        }

        visu.draw_image( 0, warp).display( disp.resize().wait( 30));

        t += 1.0f / nb;

        if(t < 0)
        {
            t = 0;
            nb = -nb;
        }

        if(t > 1)
        {
            t = 1;
            nb = -nb;
        }
    }
}

// Block matching
float max_diff( const CImg<>& I1, const CImg<>& I2, const Imath::Box2i& area, const Imath::V2i& off)
{
    float max_diff = 0;

    for(int j = area.min.y; j <= area.max.y; ++j)
        for(int i = area.min.x; i <= area.max.x; ++i)
            max_diff = std::max(max_diff, abs(I1(i, j) - I2(i + off.x, j + off.y)));

    return max_diff;
}

float ssd(const CImg<>& I1, const Imath::V2i& patch_pos, int patch_w, int patch_h, const CImg<>& I2, const Imath::V2i& search_pos)
{
    float result = 0;

    for(int j = 0; j < patch_h; ++j)
        for(int i = 0; i < patch_w; ++i)
            result += square(I1(patch_pos.x + i, patch_pos.y + j) - I2(search_pos.x + i, search_pos.y + j));

    return result;
}

void ssd_search( const CImg<>& I1, const Imath::Box2i& patch_area, const CImg<>& I2, const Imath::Box2i& search_area, Imath::V2i& min_ssd_pos)
{
    min_ssd_pos.x = min_ssd_pos.y = 0;

    int patch_w = patch_area.size().x;
    int patch_h = patch_area.size().y;

    int search_w = search_area.size().x - patch_w;
    int search_h = search_area.size().y - patch_h;

    float min_ssd = std::numeric_limits<float>::max();

    for(int j = search_area.min.y; j <= search_area.min.y + search_h; ++j)
    {
        for(int i = search_area.min.x; i <= search_area.min.x + search_w; ++i)
        {
            float c = ssd(I1, patch_area.min, patch_w, patch_h, I2, Imath::V2i(i, j));
            //float dweight = abs( i - patch_area.min.x) + abs( j - patch_area.min.y);
            //c *= dweight;

            if(c < min_ssd)
            {
                min_ssd = c;
                min_ssd_pos.x = i;
                min_ssd_pos.y = j;
            }
        }
    }
}

void do_block_match_optflow( const tbb::blocked_range<int>& r, const CImg<>& I1, const CImg<>& I2, CImg<>& U, int block_radius, int search_radius)
{
    int height = I1.height();
    int width = I1.width();

    for(int j = r.begin(); j < r.end(); ++j)
    {
        for(int i = 0; i < width; ++i)
        {
            Imath::Box2i patch_area;
            patch_area.min.x = clamp(i - block_radius, 0, width - 1);
            patch_area.min.y = clamp(j - block_radius, 0, height - 1);
            patch_area.max.x = clamp(i + block_radius, 0, width - 1);
            patch_area.max.y = clamp(j + block_radius, 0, height - 1);

            Imath::V2i v(U(i, j, 0, 0), U(i, j, 0, 1));

            Imath::Box2i search_area;
            search_area.min.x = clamp(i + v.x - search_radius, 0, width - 1);
            search_area.min.y = clamp(j + v.y - search_radius, 0, height - 1);
            search_area.max.x = clamp(i + v.x + search_radius, 0, width - 1);
            search_area.max.y = clamp(j + v.y + search_radius, 0, height - 1);

            if( search_area.isEmpty())
                return;

            // adjust patch area in case is larger than search area.
            Imath::Box2i b = offset_by(search_area, -v);
            patch_area = intersect(patch_area, b);

            if( patch_area.isEmpty())
                return;

            if(max_diff(I1, I2, patch_area, v) > 0.025f)
            {
                v.x = v.y = 0;
                ssd_search(I1, patch_area, I2, search_area, v);
                U(i, j, 0, 0) = v.x - patch_area.min.x;
                U(i, j, 0, 1) = v.y - patch_area.min.y;
            }
        }
    }
}

void block_match_optflow( const CImg<>& I1, const CImg<>& I2, CImg<>& U, int block_radius, int search_radius)
{    
    tbb::parallel_for(tbb::blocked_range<int>(0, I1.height()),
                      boost::bind( do_block_match_optflow, _1, boost::cref(I1),
                                  boost::cref(I2), boost::ref(U), block_radius, search_radius));
}

// TVL1

// derivatives & divergence
inline float dx( const CImg<>& I, int x, int y, int c = 0)
{
    return I( std::min( x + 1, I.width() - 1), y, 0, c) - I( x, y, 0, c);
}

inline float dy( const CImg<>& I, int x, int y, int c = 0)
{
    return I( x, std::min( y + 1, I.height() - 1), 0, c) - I( x, y, 0, c);
}

inline float dxm( const CImg<>& I, int x, int y, int c = 0)
{
    if( x == 0)
        return I( x, y, 0, c);

    if( x == I.width() - 1)
        return -I( x - 1, y, 0, c);

    return I( x, y, 0, c) - I( x - 1, y, 0, c);
}

inline float dym( const CImg<>& I, int x, int y, int c = 0)
{
    if( y == 0)
        return I( x, y, 0, c);

    if( y == I.height() - 1)
        return -I( x, y - 1, 0, c);

    return I( x, y, 0, c) - I( x, y - 1, 0, c);
}

inline float divergence( const CImg<>& p, int x, int y, float& divu, float& divv, float& divw)
{
    divu = dxm( p, x, y, 0) + dym( p, x, y, 1);
    divv = dxm( p, x, y, 2) + dym( p, x, y, 3);
    divw = dxm( p, x, y, 4) + dym( p, x, y, 5);
}

// warp
void warping( const tbb::blocked_range<int>& r, const CImg<>& I1, const CImg<>& I2, const CImg<>& flow, CImg<>& I2_warped, CImg<>& Ix, CImg<>& Iy, CImg<>& It)
{
    for( int j = r.begin(), je = Ix.height(); j < r.end(); ++j)
    {
        for( int i = 0, ie = Ix.width(); i < ie; ++i)
        {
            float u = i + flow( i, j, 0, 0);
            float v = j + flow( i, j, 0, 1);

            I2_warped( i, j) = I2.cubic_atXY( u, v);

            if( u < 1 || u > ie - 2 || v < 1 || v > je - 2)
            {
                Ix( i, j) = 0;
                Iy( i, j) = 0;
                It( i, j) = 0;
            }
            else
            {
                float ix = I2.cubic_atXY( u + 0.5f, v) - I2.cubic_atXY( u - 0.5f, v);
                float iy = I2.cubic_atXY( u, v + 0.5f) - I2.cubic_atXY( u, v - 0.5f);

                if( i > 0 && i < ie - 1 && j > 0 && j < je - 1)
                {
                    float i1x = I1( i + 1, j) - I1( i - 1, j);
                    float i1y = I1( i, j + 1) - I1( i, j - 1);

                    ix = ( ix + i1x) * 0.5f;
                    iy = ( iy + i1y) * 0.5f;
                }

                // avoid zero gradients
                float const eps = 0.001f;

                if( abs( ix) < eps)
                    ix = sign( ix) * eps;

                if( abs( iy) < eps)
                    iy = sign( iy) * eps;
                
                Ix( i, j) = ix;
                Iy( i, j) = iy;
                It( i, j) = I2_warped( i, j) - I1( i, j);
            }
        }
    }
}

void update_dual( const CImg<>& flow, const CImg<>& w, CImg<>& p)
{
    float L = 2.828427125f; // sqrt( 8)
    float sigma = 1.0f / L;
    float epsilon_u = 0;
    float epsilon_w = 0;

    for( int j = 0, je = p.height(); j < je; ++j)
    {
        for(int i = 0, ie = p.width(); i < ie; ++i)
        {
            float u_x = dx( flow, i, j, 0);
            float u_y = dy( flow, i, j, 0);

            float v_x = dx( flow, i, j, 1);
            float v_y = dy( flow, i, j, 1);

            float w_x = dx( w, i, j);
            float w_y = dy( w, i, j);

            p( i, j, 0, 0) = ( p( i, j, 0, 0) + sigma * u_x) / (1.0f + sigma * epsilon_u);
            p( i, j, 0, 1) = ( p( i, j, 0, 1) + sigma * u_y) / (1.0f + sigma * epsilon_u);
            p( i, j, 0, 2) = ( p( i, j, 0, 2) + sigma * v_x) / (1.0f + sigma * epsilon_u);
            p( i, j, 0, 3) = ( p( i, j, 0, 3) + sigma * v_y) / (1.0f + sigma * epsilon_u);
            p( i, j, 0, 4) = ( p( i, j, 0, 4) + sigma * w_x) / (1.0f + sigma * epsilon_w);
            p( i, j, 0, 5) = ( p( i, j, 0, 5) + sigma * w_y) / (1.0f + sigma * epsilon_w);

            // reprojection to |pu| <= 1
            float reproj = std::max( std::sqrt( square( p( i, j, 0, 0)) + square( p( i, j, 0, 1)) +
                                               square( p( i, j, 0, 2)) + square( p( i, j, 0, 3))), 1.0f);

            p( i, j, 0, 0) /= reproj;
            p( i, j, 0, 1) /= reproj;
            p( i, j, 0, 2) /= reproj;
            p( i, j, 0, 3) /= reproj;

            reproj = std::max( std::sqrt( square( p( i, j, 0, 4)) + square( p( i, j, 0, 5))), 1.0f);
            p( i, j, 0, 4) /= reproj;
            p( i, j, 0, 5) /= reproj;
        }
    }
}
void peak_filter( CImg<>& flow, CImg<>& tmp)
{
    tmp = flow.get_blur_median(3);
    double mean_x = 0, mean_y = 0;

    // calc mean
    for(int j = 0, je = flow.height(); j < je; ++j)
    {
        for(int i = 0, ie = flow.width(); i < ie; ++i)
        {
            mean_x += std::fabs( tmp( i, j, 0, 0));
            mean_y += std::fabs( tmp( i, j, 0, 1));
        }
    }

    mean_x /= flow.width() * flow.height();
    mean_y /= flow.width() * flow.height();

    for(int j = 0, je = flow.height(); j < je; ++j)
    {
        for(int i = 0, ie = flow.width(); i < ie; ++i)
        {
            float diffx = std::fabs( flow( i, j, 0, 0) - tmp( i, j, 0, 0));
            float diffy = std::fabs( flow( i, j, 0, 1) - tmp( i, j, 0, 1));

            if(diffx > mean_x)
                flow( i, j, 0, 0) = tmp( i, j, 0, 0);

            if(diffy > mean_y)
                flow( i, j, 0, 1) = tmp( i, j, 0, 1);
        }
    }
}

void tvl1_motion_primal_dual( const CImg<>& I1, const CImg<>& I2, CImg<>& flow, CImg<>& w, CImg<>& p, float lambda,
                             int num_warps, int max_iters, float scale, CImgDisplay& disp)
{
    float L = 2.828427125f; // sqrt( 8)
    float tau = 1.0f / L;
    float sigma = tau;
    float gamma = 0.02f;

    CImg<> Ix( I1.width(), I1.height());
    CImg<> Iy( I1.width(), I1.height());
    CImg<> It( I1.width(), I1.height());
    CImg<> I2_warped( I1.width(), I1.height());
    CImg<> flow_( flow, false);
    CImg<> w_( w, false);
    CImg<> tmp( flow, false);

    for( int warp = 0; warp < num_warps; ++warp)
    {
        CImg<> flow0( flow, false);

        tbb::parallel_for( tbb::blocked_range<int>(0, I1.height()),
                              boost::bind( warping, _1, boost::cref( I1), boost::cref( I2),
                                            boost::cref( flow0), boost::ref( I2_warped),
                                            boost::ref( Ix), boost::ref( Iy), boost::ref( It)));

        if( disp)
        {
            disp.resize();

            if( disp.is_closed())
                std::exit( 0);

            const unsigned char white[] = { 255,255,255 };
            CImg<unsigned char> tmp = I2_warped.get_normalize( 0,255);
            tmp.resize(disp.width(),disp.height()).draw_quiver( flow0, white, 0.7f, 15, -14, true).display( disp);
        }

        for( int iter = 0; iter < max_iters; ++iter)
        {
            update_dual( flow_, w_, p);

            for( int j = 0, je = Ix.height(); j < je; ++j)
            {
                for( int i = 0, ie = Ix.width(); i < ie; ++i)
                {
                    // primal
                    float div_u, div_v, div_w;
                    divergence( p, i, j, div_u, div_v, div_w);

                    // save current values
                    flow_( i, j, 0, 0) = flow( i, j, 0, 0);
                    flow_( i, j, 0, 1) = flow( i, j, 0, 1);
                    w_( i, j) = w( i, j);

                    // update flow
                    flow( i, j, 0, 0)   += tau * div_u;
                    flow( i, j, 0, 1)   += tau * div_v;
                    w( i, j)            += tau * div_w;

                    // thereshold
                    float rho = It( i, j) + ( flow( i, j, 0, 0) - flow0( i, j, 0, 0)) * Ix( i, j) +
                                            ( flow( i, j, 0, 1) - flow0( i, j, 0, 1)) * Iy( i, j) + gamma * w( i, j);

                    float grad_sqr = std::max( square( Ix( i, j)) + square( Iy( i, j)) + gamma * gamma, 1e-09f);

                    if( rho < - tau * lambda * grad_sqr)
                    {
                        flow( i, j, 0, 0) += tau * lambda * Ix( i, j);
                        flow( i, j, 0, 1) += tau * lambda * Iy( i, j);
                        w( i, j) += tau * lambda * gamma;
                    }
                    else
                    {
                        if( rho > tau * lambda * grad_sqr)
                        {
                            flow( i, j, 0, 0) -= tau * lambda * Ix( i, j);
                            flow( i, j, 0, 1) -= tau * lambda * Iy( i, j);
                            w( i, j) -= tau * lambda * gamma;
                        }
                        else
                        {
                            if( std::fabs( rho) <= tau * lambda * grad_sqr)
                            {
                                flow( i, j, 0, 0) -= rho * Ix( i, j) / grad_sqr;
                                flow( i, j, 0, 1) -= rho * Iy( i, j) / grad_sqr;
                                w( i, j) -= rho * gamma / grad_sqr;
                            }
                        }
                    }

                    flow_( i, j, 0, 0) = 2.0f * flow( i, j, 0, 0) - flow_( i, j, 0, 0);
                    flow_( i, j, 0, 1) = 2.0f * flow( i, j, 0, 1) - flow_( i, j, 0, 1);
                    w_( i, j) = 2.0f * w( i, j) - w_( i, j);
                }
            }

            peak_filter( flow, tmp);

            if( disp && ( iter % 10) == 0)
            {
                if( disp.is_closed())
                    std::exit( 0);

                const unsigned char white[] = { 255,255,255};
                CImg<unsigned char> tmp = I2_warped.get_normalize( 0,255);
                tmp.resize( disp.width(), disp.height()).draw_quiver( flow, white, 0.7f, 15, -14, true).display( disp);
            }
        }
    }
}

void multiscale_optical_flow( const CImg<>& I1, const CImg<>& I2, float scale_factor,
                                float lambda, int num_warps, int max_iters, int block_levels, int block_size, int block_search,
                                CImgDisplay& disp, CImg<>& flow, CImg<>& w)
{

    if( scale_factor > 0.995f)
    {
        flow = CImg<>( I1.width(), I1.height(), 1, 2, 0);
        w = CImg<>( I1.width(), I1.height(), 1, 1, 0);
        CImg<> p( I1.width(), I1.height(), 1, 6, 0);
        tvl1_motion_primal_dual( I1, I2, flow, w, p, lambda, num_warps, max_iters, 1, disp);
    }
    else
    {
        std::vector<std::pair<int, int> > level_sizes;
        level_sizes.push_back( std::make_pair( I1.width(), I1.height()));

        for( int i = 1; i < 200; ++i)
        {
            int w = level_sizes[i-1].first * scale_factor;
            int h = level_sizes[i-1].second * scale_factor;

            if( w < 16 || h < 16)
                break;

            level_sizes.push_back( std::make_pair( w, h));
        }

        CImg<> p;

        for( int level = level_sizes.size() - 1; level >= 0; --level)
        {
            float scale = std::pow( scale_factor, level);

            CImg<> pI1 = I1.get_resize( level_sizes[level].first, level_sizes[level].second, 1, 1, 2);
            CImg<> pI2 = I2.get_resize( level_sizes[level].first, level_sizes[level].second, 1, 1, 2);

            if( level == level_sizes.size() - 1)
            {
                flow = CImg<>( level_sizes[level].first, level_sizes[level].second, 1, 2, 0);
                w = CImg<>( level_sizes[level].first, level_sizes[level].second, 1, 1, 0);
                p = CImg<>( level_sizes[level].first, level_sizes[level].second, 1, 6, 0);
            }
            else
            {
                flow.resize( level_sizes[level].first, level_sizes[level].second, 1, 2, 3);
                flow /= scale_factor;

                w.resize( level_sizes[level].first, level_sizes[level].second, 1, 1, 3);
                p.resize( level_sizes[level].first, level_sizes[level].second, 1, 6, 3);
            }

            if( block_levels != 0 && ( level_sizes.size() - level <= block_levels))
            {
                std::cout << "Level = " << level << ", Size = " 
                            << level_sizes[level].first << ", " << level_sizes[level].second
                            << " Block Matching\n";

                block_match_optflow( pI1, pI2, flow, block_size, block_search);
            }
            else
            {
                std::cout << "Level = " << level << ", Size = "
                            << level_sizes[level].first << ", " << level_sizes[level].second
                            << " Variational\n";

                tvl1_motion_primal_dual( pI1, pI2, flow, w, p, lambda, num_warps, max_iters, scale, disp);
            }
        }
    }
}

/*******************************************/

void smooth_flow( const CImg<>& I1, const CImg<>& I2, float smoothness, float precision, const CImgList<>& dI, CImg<>& U, int max_iters)
{
    int sw = I1.width();
    int sh = I1.height();
    float dt = 2;

    float energy = cimg::type<float>::max();
    float _precision = (float) std::pow(10.0, -(double) precision);

    for(unsigned int iteration = 0; iteration < max_iters; ++iteration)
    {
        float _energy = 0;
        float denom = 1.0f / (4.0f * smoothness * dt + 1.0f);

        cimg_for3XY(U, x, y)
        {
            float X = x + U(x, y, 0);
            float Y = y + U(x, y, 1);

            float _energy_regul = 0;

            float deltaI = (float) (I1(x, y) - I2.linear_atXY(X, Y));

            cimg_forC(U, c)
            {
                const float Ux = 0.5f * (U(_n1x, y, c) - U(_p1x, y, c));
                const float Uy = 0.5f * (U(x, _n1y, c) - U(x, _p1y, c));
                const float Uxx = U(_n1x, y, c) + U(_p1x, y, c);
                const float Uyy = U(x, _n1y, c) + U(x, _p1y, c);

                U(x, y, c) = (float) (U(x, y, c) + dt * (deltaI * dI[c].linear_atXY(X, Y) + smoothness * (Uxx + Uyy))) * denom;
                _energy_regul += Ux * Ux + Uy * Uy;
            }

            _energy += deltaI * deltaI + smoothness * _energy_regul;
        }

        float d_energy = (_energy - energy) / (sw * sh);

        if(d_energy <= 0 && -d_energy < _precision)
            break;

        if(d_energy > 0)
            dt *= 0.5f;

        energy = _energy;
    }
}

/******************************************/

int main( int argc, char **argv)
{
    tbb::task_scheduler_init init;
    init.initialize( 2);

    // Read command line parameters
    cimg_usage("Compute an optical flow between two images");
    const char *name_i1 = cimg_option("-i" , "/home/est/Devel/opflow/eval-data/Urban/frame10.png", "Input Image 1");
    const char *name_i2 = cimg_option("-i2", "/home/est/Devel/opflow/eval-data/Urban/frame11.png", "Input Image 2");

    float detail = cimg_option( "-detail", 0.5f, "Vectors detail");
    //float factor = cimg_option( "-factor", 0.5f, "Scale factor");
    float factor = 0.75f;

    int num_warps = cimg_option( "-warps", 3, "Num warps");
    int iters = cimg_option( "-iters", 15, "Num iters");
    float lambda = cimg_option( "-lambda", 30, "Lambda");
    
    int blevels = cimg_option( "-blevels", 0, "Block levels");
    int bsize = cimg_option( "-bsize", 3, "Block size");
    int bsearch = cimg_option( "-bsearch", 6, "Block search size");

    int smooth_iters = cimg_option( "-smooth", 0, "Smooth iters");

    CImg<> tmp( name_i1);
    CImg<> I1( tmp.width(), tmp.height(), 1, 1);
    convert_to_bw( tmp, I1);

    tmp = CImg<>( name_i2);
    CImg<> I2( tmp.width(), tmp.height(), 1, 1);
    convert_to_bw( tmp, I2);

    tmp = CImg<>();

    CImgDisplay disp;
    disp.assign( I1.width(), I1.height(), "TVL1 Flow", 0);

    detail = clamp( detail, 0.2f, 1.0f);

    CImg<> flow, illum;

    if( detail == 1.0f)
    {
        multiscale_optical_flow( I1, I2, 1.0f, lambda, num_warps, iters,
                               blevels, bsize, bsearch, disp, flow, illum);

	if( smooth_iters != 0)
	{
	    CImgList<> dI = I2.get_gradient();
		smooth_flow( I1, I2, 0.1, 6.0, dI, flow, smooth_iters);
	}
    }
    else
    {
        CImg<> sI1 = I1.get_resize( I1.width() * detail, I1.height() * detail, 1, 1, 2);
        CImg<> sI2 = I2.get_resize( I2.width() * detail, I2.height() * detail, 1, 1, 2);

        multiscale_optical_flow( sI1, sI2, factor, lambda, num_warps, iters,
                                blevels, bsize, bsearch, disp, flow, illum);

	if( smooth_iters != 0)
	{
	    CImgList<> dI = sI2.get_gradient();
		smooth_flow( sI1, sI2, 0.1, 6.0, dI, flow, smooth_iters);
	}

        flow.resize( I1.width(), I1.height(), 1, 2, 5);
        flow *= ( 1.0f / detail);

        illum.resize( I1.width(), I1.height(), 1, 1, 3);
    }

    CImg<unsigned char> dispI1 = I1.get_normalize( 0, 255);
    CImg<unsigned char> dispI2 = I2.get_normalize( 0, 255);

    animate_warp( dispI1, dispI2, flow, 40, disp);

    while( 1)
    {
        if( disp.is_closed() || disp.is_keyQ())
            break;

        cimg::wait( 250);
    }

    // visualize flow
    CImgDisplay disp2;

    CImg<unsigned char> color_flow( flow.width(), flow.height(), 1, 3);

    for( int j = 0; j < color_flow.height(); ++j)
    {
	for( int i = 0; i < color_flow.width(); ++i)
	{
            unsigned char pix[3];
            compute_color( flow( i, j, 0, 0), flow( i, j, 0, 1), pix);
            color_flow( i, j, 0, 0) = pix[0];
            color_flow( i, j, 0, 1) = pix[1];
            color_flow( i, j, 0, 2) = pix[2];
	}
    }

    const unsigned char white[] = { 255,255,255};
    color_flow.draw_quiver( flow, white, 0.7f, 15, -14, true).display( disp2);

    while( 1)
    {
        if( disp2.is_closed() || disp2.is_keyQ())
            break;

        cimg::wait( 250);
    }

    /*
    // visualize illum
    CImgDisplay disp3;
    illum.get_normalize( 0, 255).display( disp3);

    while( 1)
    {
        if( disp3.is_closed() || disp3.is_keyQ())
            break;

        cimg::wait( 250);
    }
     */

    return 0;
}
