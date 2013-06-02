// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_IMAGE_GAUSSIAN_PYRAMID_HPP
#define RAMEN_IMAGE_GAUSSIAN_PYRAMID_HPP

#include<ramen/image/pyramid.hpp>

namespace ramen
{
namespace image
{

class gray_gaussian_pyramid_t : gray_pyramid_t
{
public:
	
	gray_gaussian_pyramid_t( int width, int height, float ratio = 0.5f, int min_width = 50);		
	void build();
};

class rgb_gaussian_pyramid_t : rgb_pyramid_t
{
public:

	rgb_gaussian_pyramid_t( int width, int height, float ratio = 0.5f, int min_width = 50);
	void build();
};

class rgba_gaussian_pyramid_t : rgba_pyramid_t
{
public:

	rgba_gaussian_pyramid_t( int width, int height, float ratio = 0.5f, int min_width = 50);
	void build();
};
	
} // namespace
} // namespace

#endif
