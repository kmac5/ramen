// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/image/input/input_node.hpp>

#include<stdlib.h>

#include<sstream>

#include<boost/bind.hpp>

#include<OpenEXR/ImathFun.h>

#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>
#include<ramen/algorithm/clamp.hpp>

#include<ramen/image/ocio_transform.hpp>
#include<ramen/image/color_bars.hpp>

#include<ramen/nodes/image/input/image_seq_param.hpp>
#include<ramen/nodes/image/input/image_channels_param.hpp>
#include<ramen/params/ocio_colorspace_param.hpp>
#include<ramen/params/aspect_ratio_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/string_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/separator_param.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>
#include<ramen/app/preferences.hpp>

#include<ramen/undo/stack.hpp>
#include<ramen/nodes/image/input/image_input_command.hpp>

#include<ramen/imageio/factory.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/serialization/yaml_node.hpp>
#include<ramen/serialization/yaml_oarchive.hpp>

#include<iostream>

namespace ramen
{
namespace image
{
namespace
{

enum
{
	hold_frame = 0,
	loop_frame,
	black_frame
};

} // unnamed

const int input_node_t::num_proxy_levels = 3;
	
input_node_t::input_node_t() : image_node_t()
{
    set_name( "image_in");
    add_output_plug();
    param_set().param_changed.connect( boost::bind( &input_node_t::param_changed, this, _1, _2));	
}

input_node_t::input_node_t( const boost::filesystem::path& path, bool sequence,
										const boost::filesystem::path& from_dir) : image_node_t()
{
    set_name( "image_in");
    add_output_plug();
	do_create_params();
	
	filesystem::path_sequence_t seq( path, sequence);
	
	if( sequence)
	{
		boost::filesystem::path abs_dir( path.parent_path());
	
		if( path.is_relative())
			abs_dir = filesystem::make_absolute_path( abs_dir, from_dir);

		boost::filesystem::directory_iterator it( abs_dir);
		seq.add_paths( it);
	}
	
	clips_[0] = input_clip_t( seq);
	create_reader( 0, from_dir);

    param_set().param_changed.connect( boost::bind( &input_node_t::param_changed, this, _1, _2));	
}

input_node_t::input_node_t( const input_node_t& other) : image_node_t( other), clips_( other.clips_)
{
	image_seq_param_t *seq = dynamic_cast<image_seq_param_t*>( &param( "path"));
	seq->file_picked.connect( boost::bind( &input_node_t::file_picked, this, _1, _2, _3, _4));

	seq = dynamic_cast<image_seq_param_t*>( &param( "proxy1"));
	seq->file_picked.connect( boost::bind( &input_node_t::file_picked, this, _1, _2, _3, _4));

	seq = dynamic_cast<image_seq_param_t*>( &param( "proxy2"));
	seq->file_picked.connect( boost::bind( &input_node_t::file_picked, this, _1, _2, _3, _4));

	readers_.reserve( num_proxy_levels);
	for( int i = 0; i < num_proxy_levels; ++i)
	{
		readers_.push_back( boost::shared_ptr<movieio::reader_t>());
		
		if( other.readers_[i])
			create_reader( i);
	}

    param_set().param_changed.connect( boost::bind( &input_node_t::param_changed, this, _1, _2));
}

boost::tuple<int,int,int,int> input_node_t::get_channels() const
{
	return get_value<tuple4i_t>( param( "channels"));
}

void input_node_t::set_channels( const boost::tuple<int,int,int,int>& c)
{
	image_channels_param_t *ch = dynamic_cast<image_channels_param_t*>( &param( "channels"));
	RAMEN_ASSERT( ch);
	ch->set_channels( c);	
}

void input_node_t::set_channels( const std::string& red, const std::string& green,
										const std::string& blue, const std::string& alpha)
{
	image_channels_param_t *ch = dynamic_cast<image_channels_param_t*>( &param( "channels"));
	RAMEN_ASSERT( ch);
	ch->set_channels( red, green, blue, alpha);
}

float input_node_t::get_aspect_param_value() const
{
	return get_value<float>( param( "aspect"));
}

void input_node_t::set_aspect_param_value( float a)
{
	aspect_ratio_param_t *asp = dynamic_cast<aspect_ratio_param_t*>( &param( "aspect"));
	RAMEN_ASSERT( asp);
	asp->set_value( a);
}

void input_node_t::do_create_params()
{
	if( param_set().empty())
	{
		create_image_params();
		create_more_params();
	
		clips_.reserve( num_proxy_levels);
		readers_.reserve( num_proxy_levels);
	
		for( int i = 0; i < num_proxy_levels; ++i)
		{
			clips_.push_back( input_clip_t());
			readers_.push_back( boost::shared_ptr<movieio::reader_t>());
		}
	}
}

void input_node_t::create_image_params( const boost::filesystem::path& p)
{
	std::auto_ptr<image_seq_param_t> seq( new image_seq_param_t( "Path", 0));
	seq->set_id( "path");
	seq->set_include_in_hash( false);
	seq->set_persist( false);
	seq->file_picked.connect( boost::bind( &input_node_t::file_picked, this, _1, _2, _3, _4));
	add_param( seq);

	seq.reset( new image_seq_param_t( "Proxy 1", 1));
	seq->set_id( "proxy1");
	seq->set_include_in_hash( false);
	seq->set_persist( false);
	seq->set_enabled( false);
	seq->file_picked.connect( boost::bind( &input_node_t::file_picked, this, _1, _2, _3, _4));
	add_param( seq);

	seq.reset( new image_seq_param_t( "Proxy 2", 2));
	seq->set_id( "proxy2");
	seq->set_include_in_hash( false);
	seq->set_persist( false);
	seq->set_enabled( false);
	seq->file_picked.connect( boost::bind( &input_node_t::file_picked, this, _1, _2, _3, _4));
	add_param( seq);
	
	std::auto_ptr<string_param_t> info( new string_param_t( "Info", true));
	info->set_id( "info");
	info->set_include_in_hash( false);
	info->set_persist( false);
	info->set_default_value( "no sequence");
	add_param( info);
}

void input_node_t::create_more_params()
{
	std::auto_ptr<image_channels_param_t> ch( new image_channels_param_t());
	ch->set_id( "channels");
	ch->set_include_in_hash( false);
	ch->set_persist( false);
	add_param( ch);

	std::auto_ptr<aspect_ratio_param_t> r( new aspect_ratio_param_t( "Aspect Ratio"));
	r->set_id( "aspect");
	r->set_default_value( 1.0f);
	add_param( r);
	
	std::auto_ptr<ocio_colorspace_param_t> q( new ocio_colorspace_param_t( "Colorspace"));
	q->set_id( "colorspace");
	add_param( q);	

	std::auto_ptr<separator_param_t> sep( new separator_param_t());
	add_param( sep);

	std::auto_ptr<popup_param_t> pop( new popup_param_t( "Out of Range"));
	pop->set_id( "out_range");
	pop->menu_items() = boost::assign::list_of( "Hold")( "Loop")( "Black");
	pop->set_include_in_hash( false);
	add_param( pop);
	
	std::auto_ptr<float_param_t> f( new float_param_t( "Slip"));
	f->set_id( "slip");
	f->set_default_value( 0);
	f->set_static( true);
	f->set_round_to_int( true);
	f->set_include_in_hash( false);
	add_param( f);

	std::auto_ptr<bool_param_t> b( new bool_param_t( "Lock"));
	b->set_id( "lock");
	b->set_default_value( false);
	b->set_include_in_hash( false);
	add_param( b);
}

void input_node_t::param_changed( param_t *p, param_t::change_reason reason)
{
	if( reason != param_t::time_changed)
	{
		if( p == &param( "lock") || p == &param( "slip") || p == &param( "out_range"))
			do_set_frame( composition()->frame());
	}
}

bool input_node_t::do_is_valid() const { return readers_[0];}

void input_node_t::do_set_frame( float t)
{
	boost::optional<int> frame_num = map_frame_to_sequence( t);
	
	if( frame_num)
	{
		for( int i = 0; i < num_proxy_levels; ++i)
		{
			if( readers_[i])
				readers_[i]->set_frame( frame_num.get());
		}
	}
}

boost::optional<int> input_node_t::map_frame_to_sequence( float t) const
{
	if( !readers_[0])
		return boost::optional<int>();
	
	if( !readers_[0]->is_sequence())
		return 0;
	
	int slip = get_value<float>( param( "slip"));
	
	if( get_value<bool>( param( "lock")))
		return readers_[0]->start_frame() + slip;

	int fnum = t + slip;
	
	if( fnum < readers_[0]->start_frame() || fnum > readers_[0]->end_frame())
	{
		switch( get_value<int>( param( "out_range")))
		{
			case hold_frame:
                fnum = clamp( fnum, readers_[0]->start_frame(), readers_[0]->end_frame());
			break;
			
			case loop_frame:
			{
				int num_frames = readers_[0]->end_frame() - readers_[0]->start_frame() + 1;
				fnum -= readers_[0]->start_frame();
				
				int mod = Imath::modp( fnum, num_frames);				
				return mod + readers_[0]->start_frame();
			}
			break;
		
			case black_frame:
				return boost::optional<int>();
			break;
		}
	}
	
	return fnum;
}

void input_node_t::do_calc_hash_str( const render::context_t& context)
{
	image_node_t::do_calc_hash_str( context);
	hash_generator() << context.subsample;
	
	int level = index_for_proxy_level( context.proxy_level);
	
	boost::optional<int> frame_num = map_frame_to_sequence( context.frame);

	if( frame_num)
	{
		hash_generator() << readers_[level]->string_for_current_frame();
		
		if( readers_[0]->is_multichannel())
		{
			tuple4i_t channels = get_value<tuple4i_t>( param( "channels"));
			hash_generator()	<< boost::get<0>( channels) << ", "
								<< boost::get<1>( channels) << ", "
								<< boost::get<2>( channels) << ", "
								<< boost::get<3>( channels);
		}
	}
	else
		hash_generator() << "[black]";
}

void input_node_t::do_calc_format( const render::context_t& context)
{
	Imath::Box2i format( readers_[index_for_proxy_level( context.proxy_level)]->format());
	
	if( format.isEmpty())
	{
		// init with default values
		set_format( Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( context.default_format.area().max.x - 1, 
																 context.default_format.area().max.y - 1)));
		set_aspect_ratio( context.default_format.aspect);
		set_proxy_scale( Imath::V2f( 1.0f, 1.0f));
		return;
	}
	
	Imath::Box2i full_format( readers_[0]->format());
	
    set_format( format);
	set_aspect_ratio( get_value<float>( param( "aspect")));
	set_proxy_scale( Imath::V2f( ( float) format.size().x / full_format.size().x,
								 ( float) format.size().y / full_format.size().y));
}

void input_node_t::do_calc_bounds( const render::context_t& context)
{
	Imath::Box2i bounds( readers_[index_for_proxy_level( context.proxy_level)]->bounds());
	
	if( bounds.isEmpty())
		bounds = format();
	
	set_bounds( bounds);
}

void input_node_t::do_calc_defined( const render::context_t& context)
{
	set_defined( ImathExt::intersect( bounds(), interest()));
    real_defined_ = defined();
}

void input_node_t::do_process( const render::context_t& context)
{
	boost::optional<int> frame_num = map_frame_to_sequence( context.frame);
	
	if( !frame_num)
		return;	// black border mode
	
    if( real_defined_.isEmpty())
		return;

    try
    {
		int level = index_for_proxy_level( context.proxy_level);
		
		if( readers_[level]->is_multichannel() && readers_[level]->has_extra_channels())
		{
			tuple4i_t channels = get_value<tuple4i_t>( param( "channels"));
			readers_[level]->read_frame( image_view(), real_defined_, context.subsample, channels);
		}
		else
			readers_[level]->read_frame( image_view(), real_defined_, context.subsample);

        // transform to linear colorspace here.
        OCIO::ConstConfigRcPtr config = app().ocio_manager().config();
        OCIO::ConstProcessorRcPtr proc = config->getProcessor( get_value<std::string>( param( "colorspace")).c_str(), OCIO::ROLE_SCENE_LINEAR);
        image::ocio_transform( image_view(), proc);
    }
	catch( movieio::exception& e)
	{
		image::make_color_bars( image_view());
	}
    catch( OCIO::Exception& e)
    {
		// ????
    }
}

void input_node_t::do_read( const serialization::yaml_node_t& node, const std::pair<int,int>& version)
{
	filesystem::path_sequence_t seq;
	
	if( node.get_optional_value( "images", seq))
	{
		clips_[0] = input_clip_t( seq);
		create_reader( 0);
	}

	if( node.get_optional_value( "proxy1", seq))
	{
		clips_[1] = input_clip_t( seq);
		create_reader( 1);
	}

	if( node.get_optional_value( "proxy2", seq))
	{
		clips_[2] = input_clip_t( seq);
		create_reader( 2);
	}
	
	serialization::optional_yaml_node_t channels( node.get_optional_node( "channels"));
	
	if( channels)
	{
		std::string ch0, ch1, ch2, ch3;
		
		channels.get()[0] >> ch0;
		channels.get()[1] >> ch1;
		channels.get()[2] >> ch2;
		channels.get()[3] >> ch3;
		
		// TODO: check here for errors.
		set_channels( ch0, ch1, ch2, ch3);
	}
}

void input_node_t::do_write( serialization::yaml_oarchive_t& out) const
{
	if( readers_[0])
		out << YAML::Key << "images" << YAML::Value << clips_[0].path_sequence();

	if( readers_[1])
		out << YAML::Key << "proxy1" << YAML::Value << clips_[1].path_sequence();

	if( readers_[2])
		out << YAML::Key << "proxy2" << YAML::Value << clips_[2].path_sequence();

	if( readers_[0] && readers_[0]->is_multichannel() && readers_[0]->has_extra_channels())
	{
		std::string ch0, ch1, ch2, ch3;

		const image_channels_param_t *p = dynamic_cast<const image_channels_param_t*>( &param( "channels"));
		RAMEN_ASSERT( p);
		p->get_channel_names( ch0, ch1, ch2, ch3);
		
		out << YAML::Key << "channels" << YAML::Value;
		out.begin_seq();
			out << YAML::DoubleQuoted << ch0;
			out << YAML::DoubleQuoted << ch1;
			out << YAML::DoubleQuoted << ch2;
			out << YAML::DoubleQuoted << ch3;
		out.end_seq();
	}
}

void input_node_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
	for( int i = 0; i < clips_.size(); ++i)
		clips_[i].convert_relative_paths( old_base, new_base);
}

void input_node_t::make_paths_absolute()
{
	for( int i = 0; i < clips_.size(); ++i)
		clips_[i].make_paths_absolute( composition()->composition_dir());
}

void input_node_t::make_paths_relative()
{
	for( int i = 0; i < clips_.size(); ++i)
		clips_[i].make_paths_relative( composition()->composition_dir());
}

int input_node_t::index_for_proxy_level( int proxy_level) const
{
	RAMEN_ASSERT( proxy_level >= 0 && proxy_level < num_proxy_levels);
	
	int best_level = 0;
	
	for( int i = 1; i <= proxy_level; ++i)
	{
		if( readers_[i])
			best_level = i;
	}
	
	return best_level;
}

void input_node_t::create_reader( int proxy_level)
{
	RAMEN_ASSERT( composition());
	create_reader( proxy_level, composition()->composition_dir());
}

void input_node_t::create_reader( int proxy_level, const boost::filesystem::path& from_dir)
{
	RAMEN_ASSERT( proxy_level >= 0 && proxy_level < num_proxy_levels);

    core::auto_ptr_t<movieio::reader_t> r( clips_[proxy_level].reader( from_dir));
	readers_[proxy_level].reset( r.release());
	
	if( proxy_level == 0)
	{
		string_param_t *info = dynamic_cast<string_param_t*>( &param( "info"));
		RAMEN_ASSERT( info);
		
		if( readers_[0])
		{
			image_seq_param_t *seq = dynamic_cast<image_seq_param_t*>( &param( "path"));
			RAMEN_ASSERT( seq);
			
			seq->set_input_text( clips_[0].format_string());
			
			std::stringstream ss;
			Imath::Box2i format( readers_[0]->format());
						
			if( readers_[0]->is_sequence())
				ss << "frames " << readers_[0]->start_frame() << "..." << readers_[0]->end_frame() << " ";

			if( !format.isEmpty())			
				ss << "size [" << format.size().x + 1 << " x " << format.size().y + 1 << "]";
			
			info->set_value( ss.str());
			
			image_channels_param_t *ch = dynamic_cast<image_channels_param_t*>( &param( "channels"));
			RAMEN_ASSERT( ch);
			
			if( readers_[0]->is_multichannel() && readers_[0]->has_extra_channels())
				ch->set_channel_list( readers_[0]->channel_list());
			else
				ch->clear_channel_list();
			
			set_aspect_param_value( readers_[0]->aspect_ratio());
						
			// ocio_colorspace_param_t *cs = dynamic_cast<ocio_colorspace_param_t*>( &param( ""));
			// RAMEN_ASSERT( cs);
			// cs->...( ...);			
		}
	}
	else
	{
		if( proxy_level == 1)
		{
			image_seq_param_t *seq = dynamic_cast<image_seq_param_t*>( &param( "proxy1"));
			RAMEN_ASSERT( seq);
			seq->set_input_text( clips_[1].format_string());
		}
		else
		{
			image_seq_param_t *seq = dynamic_cast<image_seq_param_t*>( &param( "proxy2"));
			RAMEN_ASSERT( seq);
			seq->set_input_text( clips_[2].format_string());
		}
	}
}

void input_node_t::file_picked( const boost::filesystem::path& p, int level, bool sequence, bool relative)
{
	std::auto_ptr<undo::image_input_command_t> c( new undo::image_input_command_t( *this, level));
	
	if( level != 0)
	{
		if( readers_[0] && !readers_[0]->is_sequence())
			sequence = false;
	}

	boost::filesystem::path abs_path( p);

	if( p.is_relative())
		abs_path = composition()->relative_to_absolute( abs_path);
	
	if( !imageio::factory_t::instance().is_image_file( abs_path))
		sequence = false;
	
	filesystem::path_sequence_t seq( p, sequence);
	
	if( sequence)
	{
		boost::filesystem::directory_iterator it( abs_path.parent_path());
		seq.add_paths( it);
	}
	
	clips_[level] = input_clip_t( seq);
	create_reader( level);
	set_frame( composition()->frame());

	notify();
	app().document().undo_stack().push_back( c);
	app().ui()->update();
}

// factory
node_t *create_image_input_node() { return new input_node_t();}

node_t *create_gui_image_input_node()
{
    bool sequence, relative;
    boost::filesystem::path p, dir;

    if( app().ui()->image_sequence_file_selector( p, sequence, relative))
        return new input_node_t( p, sequence, app().document().composition().composition_dir());

    return 0;
}

const node_metaclass_t *input_node_t::metaclass() const { return &image_input_node_metaclass();}

const node_metaclass_t& input_node_t::image_input_node_metaclass()
{
    static bool inited( false);
    static node_metaclass_t info;

    if( !inited)
    {
        info.id = "image.builtin.image_input";
        info.major_version = 1;
        info.minor_version = 0;
        info.menu = "Image";
        info.submenu = "Input";
        info.menu_item = "Image...";
        info.create = &create_image_input_node;
        info.create_gui = &create_gui_image_input_node;
        inited = true;
    }

    return info;
}

static bool registered = node_factory_t::instance().register_node( input_node_t::image_input_node_metaclass());

} // namespace
} // namespace
