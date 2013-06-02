// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/nodes/image/generate/text_node.hpp>

#include<algorithm>

#include<ramen/assert.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/params/file_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/string_param.hpp>

#include<ramen/manipulators/point2_manipulator.hpp>
#include<ramen/manipulators/draw.hpp>

namespace ramen
{
namespace
{

enum
{
	align_left = 0,
	align_center,
	align_right
};

} // unnamed
	
class text_node_manipulator_t : public manipulator_t
{
public:
	
	text_node_manipulator_t( image::text_node_t *node)
	{
		RAMEN_ASSERT( node);
		node_ = node;
	}

private:
	
	virtual void do_draw_overlay( const ui::paint_event_t& event) const {}

	image::text_node_t *node_;	
};
	
namespace image
{

text_node_t::text_node_t() : generator_node_t()
{ 
	set_name( "text");
    param_set().param_changed.connect( boost::bind( &text_node_t::param_changed, this, _1, _2));
}

text_node_t::text_node_t( const text_node_t& other) : generator_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &text_node_t::param_changed, this, _1, _2));
}

void text_node_t::do_create_params()
{
    generator_node_t::do_create_params();
	
	std::auto_ptr<file_param_t> p( new file_param_t( "Font"));	
	p->set_id( "font");
	p->set_extension_list_string( "TrueType Font (*.ttf)");
	add_param( p);

	std::auto_ptr<string_param_t> s( new string_param_t( "Text"));
	s->set_id( "text");
	s->set_multiline( true);
	add_param( s);
	
	std::auto_ptr<float2_param_t> f2( new float2_param_t( "Size"));
	f2->set_id( "size");
	f2->set_range( 0, 250);
	f2->set_default_value( Imath::V2f( 14, 14));
	f2->set_proportional( true);
	add_param( f2);
	
    std::auto_ptr<popup_param_t> align( new popup_param_t( "H. Align"));
    align->set_id( "halign");
    align->menu_items() = boost::assign::list_of( "Left")( "Centre")( "Right");
    add_param( align);

    align.reset( new popup_param_t( "V. Align"));
    align->set_id( "valign");
    align->menu_items() = boost::assign::list_of( "Left")( "Centre")( "Right");
    add_param( align);
	
	f2.reset( new float2_param_t( "Pos"));
	f2->set_id( "pos");
	f2->set_numeric_type( numeric_param_t::relative_xy);
	f2->set_default_value( Imath::V2f( 0.5f, 0.5f));
	add_param( f2);
}

void text_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	/*
	if( reason != param_t::time_changed)
	{
		file_param_t *file = dynamic_cast<file_param_t*>( &param( "font"));
		
		if( p == file)
			create_face();
	}
	*/
}

void text_node_t::do_create_manipulators()
{
	float2_param_t *p = dynamic_cast<float2_param_t*>( &param( "pos"));
	RAMEN_ASSERT( p);
	
	std::auto_ptr<point2_manipulator_t> mp( new point2_manipulator_t( p));
	add_manipulator( mp);
	
	std::auto_ptr<text_node_manipulator_t> mt( new text_node_manipulator_t( this));
	add_manipulator( mt);
}

bool text_node_t::do_is_valid() const { return false;}

//void text_node_t::do_calc_bounds( const render::context_t& context) {}

void text_node_t::do_process( const render::context_t& context) {}

// factory
node_t *create_text_node() { return new text_node_t();}

const node_metaclass_t *text_node_t::metaclass() const { return &text_node_metaclass();}

const node_metaclass_t& text_node_t::text_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.text";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Text";
        info.create = &create_text_node;
        inited = true;
    }

    return info;
}

#ifndef NDEBUG
	static bool registered = node_factory_t::instance().register_node( text_node_t::text_node_metaclass());
#endif

} // namespace
} // namespace

/*
inline float fixed_to_float( FT_Fixed f)
{
	return ( float) f / 64.0f;
}

inline FT_Fixed float_to_fixed( float f)
{
	return ( FT_Fixed) f * 0x10000L;
}
 
void text_node_t::create_face()
{
	try
	{
		face_ = freetype::font_cache_t::Instance().create_face( get_value<boost::filesystem::path>( param( "font")));
	}
	catch( freetype::exception& e)
	{
		face_.reset();
	}
}

Imath::Box2f text_node_t::calc_text_bbox( const std::string& text)
{
	RAMEN_ASSERT( face_);
	FT_Face face = face_.get();
	
	float width, height = 0;
	int left_ch = 0;
	
	float lastadvance = 0;
	float lastwidth = 0;
	float advance = 0;
	
	for( int i = 0, ie = text.size(); i < ie; ++i)
	{
		int ch = FT_Get_Char_Index( face, text[i]);
		FT_Load_Glyph( face, ch, FT_LOAD_DEFAULT);
			
		lastadvance = fixed_to_float( face->glyph->metrics.horiAdvance);

		if( FT_HAS_KERNING( face) && left_ch && ch)
		{
			FT_Vector kern;
			FT_Get_Kerning( face, left_ch, ch, FT_KERNING_DEFAULT, &kern);
			lastadvance += fixed_to_float( kern.x);
		}
		
		lastwidth = fixed_to_float( face->glyph->metrics.width);
		advance += lastadvance;
		
		height = std::max( height, fixed_to_float( face->glyph->metrics.height));
		left_ch = ch;
	}
	
	width = advance - lastadvance - lastwidth;
	
	Imath::V2f pos = get_absolute_value<Imath::V2f>( param( "pos"));
	Imath::Box2f text_area;
	
	switch( get_value<int>( param( "halign")))
	{
		case align_left:
			text_area.min.x = pos.x;
			text_area.max.x = pos.x + width;
		break;
		
		case align_center:
			text_area.min.x = pos.x - ( width / 2.0f);
			text_area.max.x = pos.x + ( width / 2.0f);
		break;
		
		case align_right:
			text_area.min.x = pos.x - width;
			text_area.max.x = pos.x;
		break;
	};

	switch( get_value<int>( param( "valign")))
	{
		case align_left:
			text_area.min.y = pos.y;
			text_area.max.y = pos.y + height;
		break;
		
		case align_center:
			text_area.min.y = pos.y - ( height / 2.0f);
			text_area.max.y = pos.y + ( height / 2.0f);
		break;
		
		case align_right:
			text_area.min.y = pos.y - height;
			text_area.max.y = pos.y;
		break;
	};
}

void text_node_t::render_text( const std::string& text, const Imath::V2f& pos)
{
	RAMEN_ASSERT( face_);
	FT_Face face = face_.get();
	
	FT_Vector pen;
	pen.x = float_to_fixed( pos.x);
	pen.y = float_to_fixed( pos.y);

	for( int i = 0, ie = text.size(); i < ie; ++i)
	{
		FT_Set_Transform( face, 0, &pen);
		
		int ch = FT_Get_Char_Index( face, text[i]);		
		FT_Error error = FT_Load_Glyph( face, ch, FT_LOAD_RENDER);

		if( error)
			continue;
		
		FT_GlyphSlot slot = face->glyph;
		//my_draw_bitmap( &slot->bitmap, slot->bitmap_left, my_target_height - slot->bitmap_top);

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}
}
*/
