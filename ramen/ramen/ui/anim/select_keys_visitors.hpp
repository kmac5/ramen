// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_ANIM_SELECT_KEYS_VISITORS_HPP
#define	RAMEN_UI_ANIM_SELECT_KEYS_VISITORS_HPP

#include<ramen/anim/any_curve.hpp>
#include<ramen/anim/curve.hpp>

namespace ramen
{
namespace ui
{

struct deselect_all_visitor : public boost::static_visitor<>
{
	deselect_all_visitor() {}
	
	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		c->select_all_keyframes( false);
	}
};

struct select_all_visitor : public boost::static_visitor<>
{
	select_all_visitor() {}
	
	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		c->select_all_keyframes( true);
	}
};

struct any_selected_visitor : public boost::static_visitor<>
{
	any_selected_visitor() : any_selected( false) {}
	
	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		if( any_selected)
			return;

		any_selected = c->any_keyframe_selected();
	}

	bool any_selected;
};

struct count_selected_visitor : public boost::static_visitor<>
{
	count_selected_visitor() {}
	
	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		count = 0;

		for( typename anim::curve_t<K>::const_iterator it( c->begin()); it != c->end(); ++it)
		{
			if( it->selected())
				++count;
		}
	}
	
	int count;
};

struct select_keyframe_visitor : public boost::static_visitor<>
{
public:

	select_keyframe_visitor( int index) : index_( index) {}
	
	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		(*c)[index_].select( true);
	}

private:

	int index_;
};

struct toggle_select_keyframe_visitor : public boost::static_visitor<>
{
public:

	toggle_select_keyframe_visitor( int index) : index_( index) {}

	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		(*c)[index_].select( true);
	}
			

private:

	int index_;
};

struct keyframe_is_selected_visitor : public boost::static_visitor<>
{
public:

	keyframe_is_selected_visitor( int index) : index_( index), selected( false) {}

	template<class K>
	void operator()( const anim::curve_t<K> *c)
	{
		selected = (*c)[index_].selected();
	}
	
	bool selected;

private:

	int index_;
};

struct get_selected_keyframe_visitor : public boost::static_visitor<>
{
public:
	
	get_selected_keyframe_visitor() : index( -1) {}

	template<class K>	
	void operator()( const anim::curve_t<K> *c)
	{
		index = -1;
		int i = 0;
			
		for( typename anim::curve_t<K>::const_iterator it( c->begin()); it != c->end(); ++it, ++i)
		{
			if( it->selected())
			{
				if( index == -1)
					index = i; // this is the first keyframe selected.
				else
				{
					index = -1; // there are more than one keyframes selected.
					break; // We are not interested anymore...
				}				
			}
		}
	}
		
	int index;
};

} // namespace
} // namespace

#endif
