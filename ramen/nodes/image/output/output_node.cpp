// Copyright (c) 2010 Esteban Tovagliari.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/output/output_node.hpp>

#include<boost/bind.hpp>
#include<boost/format.hpp>

#include<ramen/ImathExt/ImathBoxAlgo.h>

#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/file_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/group_param.hpp>
#include<ramen/params/ocio_colorspace_param.hpp>

#include<ramen/imageio/factory.hpp>
#include<ramen/imageio/enums.hpp>

namespace ramen
{
namespace image
{
namespace
{

enum output_format
{
    exr_format = 0,
    jpg_format,
    cin_format,
    dpx_format,
    hdr_format,
    tiff_format,
    tga_format,
    png_format
};

} // namespace

output_node_t::output_node_t() : base_output_node_t()
{
    set_name( "image_out");
    param_set().param_changed.connect( boost::bind( &output_node_t::param_changed, this, _1, _2));
}

output_node_t::output_node_t( const output_node_t& other) : base_output_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &output_node_t::param_changed, this, _1, _2));
}

void output_node_t::do_create_params()
{
    std::auto_ptr<file_param_t> out( new file_param_t( "Output"));
    out->set_id( "output");
    out->set_is_input( false);
    add_param( out);

    std::auto_ptr<ocio_colorspace_param_t> cs( new ocio_colorspace_param_t( "Colorspace"));
    cs->set_id( "colorspace");
    add_param( cs);

	std::auto_ptr<float_param_t> f( new float_param_t( "Priority"));
	f->set_id( "priority");
	f->set_static( true);
	f->set_default_value( 100);
	f->set_round_to_int( true);
	f->set_include_in_hash( false);
	f->set_secret( true);
	add_param( f);
	
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Format"));
    top->set_id( "format");

    // exr
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "EXR"));
        group->set_id( "exr");

        std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
        p->set_id( "exr_channels");
        p->menu_items() = boost::assign::list_of( "RGBA")( "RGB")( "Alpha");
        group->add_param( p);

        p.reset( new popup_param_t( "Type"));
        p->set_id( "exr_type");
        p->menu_items() = boost::assign::list_of( "Half")( "Float");
        group->add_param( std::auto_ptr<param_t>( p));

        p.reset( new popup_param_t( "Compress"));
        p->set_id( "exr_compress");
        p->menu_items() = boost::assign::list_of( "None")( "RLE")( "ZIPS")( "ZIP")( "PIZ")( "PXR24")( "B44")( "B44A");
        p->set_default_value( (int) 4);
        group->add_param( p);

        top->add_param( group);
    }

   // jpg
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "Jpg"));
        group->set_id( "jpg");

        std::auto_ptr<float_param_t> p( new float_param_t( "Quality"));
        p->set_id( "jpg_quality");
        p->set_static( true);
        p->set_range( 10, 100);
        p->set_default_value( 90);
        group->add_param( std::auto_ptr<param_t>( p));

        top->add_param( group);
    }

    // cin
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "Cineon"));
        group->set_id( "cineon");
        top->add_param( group);
    }

    // dpx
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "Dpx"));
        group->set_id( "dpx");
        top->add_param( group);
    }

    // hdr
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "HDR"));
        group->set_id( "hdr");
        top->add_param( group);
    }

    // tiff
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "Tiff"));
        group->set_id( "tiff");

        std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
        p->set_id( "tiff_channels");
        p->menu_items() = boost::assign::list_of( "RGBA")( "RGB");
        group->add_param( p);

        p.reset( new popup_param_t( "Type"));
        p->set_id( "tiff_type");
        p->menu_items() = boost::assign::list_of( "8 Bits")( "16 Bits")( "Float");
        group->add_param( std::auto_ptr<param_t>( p));

        p.reset( new popup_param_t( "Compress"));
        p->set_id( "tiff_compress");
        p->menu_items() = boost::assign::list_of( "None")( "LZW")( "ZIP");
        group->add_param( p);

        top->add_param( group);
    }

    // tga
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "Tga"));
        group->set_id( "tga");

        std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
        p->set_id( "tga_channels");
        p->menu_items() = boost::assign::list_of( "RGBA")( "RGB");
        group->add_param( p);

        p.reset( new popup_param_t( "Compress"));
        p->set_id( "tga_compress");
        p->menu_items() = boost::assign::list_of( "None")( "RLE");
        group->add_param( p);

        top->add_param( group);
    }

    // png
    {
        std::auto_ptr<composite_param_t> group( new composite_param_t( "Png"));
        group->set_id( "png");

        std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
        p->set_id( "png_channels");
        p->menu_items() = boost::assign::list_of( "RGBA")( "RGB")( "Alpha");
        group->add_param( p);

        top->add_param( group);
    }

    add_param( top);
}

void output_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	if( reason == param_t::time_changed)
		return;
	
    file_param_t *out = dynamic_cast<file_param_t*>( &param( "output"));
    combo_group_param_t *format = dynamic_cast<combo_group_param_t*>( &param( "format"));
    int selected_format = get_value<int>( *format);

    if( p == out)
    {
        if( !out->empty())
        {
            std::string ext = out->extension();

            if( ext.empty())
            {
               out->set_extension( extension_for_format( selected_format));
               out->update_widgets();
            }
            else
               set_format_for_extension( ext);
        }
    }
    else
    {
        if( p == format)
        {
            if( !out->empty())
            {
                out->set_extension( extension_for_format( selected_format));
                out->update_widgets();
            }
        }
    }
}

void output_node_t::do_calc_defined( const render::context_t& context)
{
    set_defined( input_as<image_node_t>()->format());
}

namespace
{

math::box2i_t convert_box( const Imath::Box2i& box)
{
    return math::box2i_t( math::point2i_t( box.min.x, box.min.y),
                          math::point2i_t( box.max.x, box.max.y));
}

} // unnamed

void output_node_t::write( const render::context_t& context)
{
    boost::filesystem::path p( get_value<boost::filesystem::path>( param( "output")));
    std::string in( p.string());
    boost::format formater( in);
    formater.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit));
    formater % ( (int) context.frame);
    p = formater.str();

    if( p.empty())
        throw( std::runtime_error( "Image output empty path"));

    std::string tag;
    core::dictionary_t params;
    params[ core::name_t( "format")] = core::variant_t( convert_box( format()));
    params[ core::name_t( "bounds")] = core::variant_t( convert_box( input_defined()));
    params[ core::name_t( "aspect")] = core::variant_t( aspect_ratio());

    switch( get_value<int>( param( "format")))
	{
		case exr_format:
			tag = "exr";
            params[ core::name_t( "channels")]	= core::variant_t( get_value<int>( param( "exr_channels")));
            params[ core::name_t( "type")]		= core::variant_t( get_value<int>( param( "exr_type")));
            params[ core::name_t( "compress")]	= core::variant_t( get_value<int>( param( "exr_compress")));
		break;
	
		case jpg_format:
			tag = "jpg";
            params[ core::name_t( "quality")]  = core::variant_t( get_value<float>( param( "jpg_quality")));
		break;
	
		case cin_format:
			tag = "cin";
		break;
	
		case dpx_format:
				tag = "dpx";
		break;
	
		case hdr_format:
			tag = "hdr";
		break;
	
		case tiff_format:
			tag = "tiff";
            params[ core::name_t( "channels")]  = core::variant_t( get_value<int>( param( "tiff_channels")));
			
			switch( get_value<int>( param( "tiff_type")))
			{
				case 0:
                    params[ core::name_t( "type")]	= core::variant_t( (int) imageio::ubyte_channel_type);
				break;
		
				case 1:
                    params[ core::name_t( "type")]	= core::variant_t( (int) imageio::ushort_channel_type);
				break;
		
				case 2:
                    params[ core::name_t( "type")] = core::variant_t( (int) imageio::float_channel_type);
				break;
				
				default:
					RAMEN_ASSERT( 0);
			}
			
			switch( get_value<int>( param( "tiff_compress")))
			{
				case 0:
                    params[ core::name_t( "compress")]  = core::variant_t( (int) imageio::none_compression);
				break;
	
				case 1:
                    params[ core::name_t( "compress")]  = core::variant_t( (int) imageio::lzw_compression);
				break;
		
				case 2:
                    params[ core::name_t( "compress")]  = core::variant_t( (int) imageio::zip_compression);
				break;
		
				default:
					RAMEN_ASSERT( 0);
			}
			
		break;
	
		case tga_format:
			tag = "tga";
            params[ core::name_t( "channels")]  = core::variant_t( get_value<int>( param( "tga_channels")));
			
			switch( get_value<int>( param( "tga_compress")))
			{
				case 0:
                    params[ core::name_t( "compress")]  = core::variant_t( (int) imageio::none_compression);
				break;
				
				case 1:
                    params[ core::name_t( "compress")]  = core::variant_t( (int) imageio::rle_compression);
				break;
	
				default:
					RAMEN_ASSERT( 0);
			}
			
		break;
	
		case png_format:
			tag = "png";
            params[ core::name_t( "channels")]  = core::variant_t( get_value<int>( param( "png_channels")));
		break;
		
		default:
			RAMEN_ASSERT( 0);
    }

	// TODO: catch exceptions here.

    core::auto_ptr_t<imageio::writer_t> writer( imageio::factory_t::instance().writer_for_tag( tag));

    if( writer.get())
        writer->write_image( p, const_image_view(), params);
}

std::string output_node_t::extension_for_format( int format) const
{
    switch( format)
    {
        case exr_format:
            return std::string( ".exr");
        break;

        case jpg_format:
            return std::string( ".jpg");
        break;

        case cin_format:
            return std::string( ".cin");
        break;

        case dpx_format:
            return std::string( ".dpx");
        break;

        case hdr_format:
            return std::string( ".hdr");
        break;

        case tiff_format:
            return std::string( ".tif");
        break;

        case tga_format:
            return std::string( ".tga");
        break;

        case png_format:
            return std::string( ".png");
        break;

        default:
            return std::string(); // to keep MSVC happy
    }
}

void output_node_t::set_format_for_extension( const std::string& ext)
{
    combo_group_param_t *format = dynamic_cast<combo_group_param_t*>( &param( "format"));
    int selected_format = get_value<int>( *format);

    if( ext == ".exr" || ext == ".EXR")
    {
        if( selected_format != exr_format)
            format->set_value( (int) exr_format);

        format->update_widgets();
        return;
    }

    if( ext == ".jpg" || ext == ".JPG" || ext == ".jpeg" || ext == ".JPEG")
    {
        if( selected_format != jpg_format)
            format->set_value( (int) jpg_format);

        format->update_widgets();
        return;
    }

    if( ext == ".cin" || ext == ".CIN")
    {
        if( selected_format != cin_format)
            format->set_value( (int) cin_format);

        format->update_widgets();
        return;
    }

    if( ext == ".dpx" || ext == ".DPX")
    {
        if( selected_format != dpx_format)
            format->set_value( (int) dpx_format);

        format->update_widgets();
        return;
    }

    if( ext == ".hdr" || ext == ".HDR" || ext == ".rgbe" || ext == ".RGBE")
    {
        if( selected_format != hdr_format)
            format->set_value( (int) hdr_format);

        format->update_widgets();
        return;
    }

    if( ext == ".tif" || ext == ".TIF" || ext == ".tiff" || ext == ".TIFF")
    {
        if( selected_format != tiff_format)
            format->set_value( (int) tiff_format);

        format->update_widgets();
        return;
    }

    if( ext == ".tga" || ext == ".TGA")
    {
        if( selected_format != tga_format)
            format->set_value( (int) tga_format);

        format->update_widgets();
        return;
    }

    if( ext == ".png" || ext == ".PNG")
    {
        if( selected_format != png_format)
            format->set_value( (int) png_format);

        format->update_widgets();
        return;
    }
}

int output_node_t::priority() const { return get_value<float>( param( "priority"));}

// factory
node_t *create_output_node() { return new output_node_t();}

const node_metaclass_t *output_node_t::metaclass() const { return &output_node_metaclass();}

const node_metaclass_t& output_node_t::output_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.img.image_seq_output";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Output";
        info.menu_item = "Image";
        info.create = &create_output_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( output_node_t::output_node_metaclass());

} // image
} // ramen
