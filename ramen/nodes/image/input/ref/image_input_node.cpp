// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/input/image_input_node.hpp>

#include<stdlib.h>

#include<sstream>

#include<boost/bind.hpp>

#include<QFileDialog>
#include<QGridLayout>
#include<QHBoxLayout>
#include<QCheckBox>

#include<ramen/image/ocio_transform.hpp>

#include<ramen/imageio/multichannel_reader.hpp>

#include<ramen/params/image_input_param.hpp>
#include<ramen/params/ocio_colorspace_param.hpp>
#include<ramen/params/aspect_ratio_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/bool_param.hpp>
#include<ramen/params/separator_param.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/composition.hpp>
#include<ramen/app/preferences.hpp>

#include<ramen/ui/user_interface.hpp>

namespace ramen
{

image_input_node_t::image_input_node_t() : image_node_t()
{
    set_name( "image_in");
    add_output_plug();

    param_set().param_changed.connect( boost::bind( &image_input_node_t::param_changed, this, _1));
}

image_input_node_t::image_input_node_t( const boost::filesystem::path& path, bool sequence) : image_node_t()
{
    set_name( "image_in");
    add_output_plug();

	float aspect = 1.0f;
	
    std::auto_ptr<image_input_param_t> p( new image_input_param_t( "Path", path, sequence, aspect));
    p->set_id( "image");
    add_param( p);

	create_more_params( aspect);
	
    param_set().param_changed.connect( boost::bind( &image_input_node_t::param_changed, this, _1));
}

image_input_node_t::image_input_node_t( const image_input_node_t& other) : image_node_t( other)
{
    param_set().param_changed.connect( boost::bind( &image_input_node_t::param_changed, this, _1));
}

void image_input_node_t::set_channels( const std::string& red, const std::string& green,
					const std::string& blue, const std::string& alpha)
{
    time_ = 1.0f;
    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
    fname_ = p->path_for_frame( time_);

	if( !fname_.empty())
	{
		if( fname_.is_relative())
		{
			assert( composition());
			fname_ = composition()->relative_to_absolute( fname_);
		}
	
		reader_.reset();
	
		try
		{
			reader_ = imageio::factory_t::Instance().reader_for_image( fname_);
		}
		catch( std::exception&)
		{
		}
	
		p->set_channels( red, green, blue, alpha);
	}
}

void image_input_node_t::param_changed( param_t *p)
{
    if( p == &param( "image"))
		set_frame( time_);
}

void image_input_node_t::do_create_params()
{
    if( param_set().empty())
    {
		std::auto_ptr<image_input_param_t> p( new image_input_param_t( "Path"));
		p->set_id( "image");
		add_param( p);
		
		create_more_params();
    }
}

void image_input_node_t::create_more_params( float aspect)
{
	std::auto_ptr<aspect_ratio_param_t> r( new aspect_ratio_param_t( "Aspect Ratio"));
	r->set_id( "aspect");
	r->set_default_value( aspect);
	add_param( r);
	
	std::auto_ptr<ocio_colorspace_param_t> q( new ocio_colorspace_param_t( "Colorspace"));
	q->set_id( "colorspace");
	add_param( q);	

	std::auto_ptr<separator_param_t> sep( new separator_param_t());
	add_param( sep);
	
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

void image_input_node_t::do_set_frame( float t)
{
	// TODO: handle slip & lock here now
    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));

	int slip = get_value<float>( param( "slip"));
	
	int fnum;
	
	if( get_value<bool>( param( "lock")))
		fnum = p->start_frame() + slip;
	else
		fnum = t + slip;
	
    fname_ = p->path_for_frame( fnum);

	if( !fname_.empty())
	{
		if( fname_.is_relative())
		{
			assert( composition());
			fname_ = composition()->relative_to_absolute( fname_);
		}
	
		reader_.reset();
	
		try
		{
			reader_ = imageio::factory_t::Instance().reader_for_image( fname_);
		}
		catch( std::exception&)
		{
		}
		
		p->update_widgets();
	}
}

bool image_input_node_t::do_is_valid() const { return reader_.get();}

void image_input_node_t::do_calc_hash_str( const render::context_t& context)
{ 
	image_node_t::do_calc_hash_str( context);
    hash_generator() << filesystem::hash_string( fname_);
    image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
    boost::tuple<int,int,int,int> channels = p->channels();
	hash_generator() << channels;
	hash_generator() << context.proxy_level;
}

void image_input_node_t::do_calc_format( const render::context_t& context)
{ 
    set_format( reader_->format());
	set_aspect_ratio( get_value<float>( param( "aspect")));
	set_proxy_scale( 1.0f);
}

void image_input_node_t::do_calc_bounds( const render::context_t& context) { set_bounds( reader_->defined());}

void image_input_node_t::do_calc_defined( const render::context_t& context)
{
	Imath::Box2i def( Imath::intersect( reader_->defined(), interest()));
	
	// limit the image size
	if( def.size().x > preferences_t::Instance().max_image_width())
		def.max.x = def.min.x + preferences_t::Instance().max_image_width();

	if( def.size().y > preferences_t::Instance().max_image_height())
		def.max.y = def.min.y + preferences_t::Instance().max_image_height();
	
    real_defined_ = def;
	set_defined( def);
}

void image_input_node_t::do_process( const render::context_t& context)
{
    if( real_defined_.isEmpty())
		return;

    try
    {
		if( imageio::multichannel_reader_t *mreader = dynamic_cast<imageio::multichannel_reader_t*>( reader_.get()))
		{
			image_input_param_t *p = dynamic_cast<image_input_param_t*>( &param( "image"));
			boost::tuple<int,int,int,int> channels = p->channels();
			mreader->read_image( image_view(), real_defined_, context.subsample, channels);
		}
		else
			reader_->read_image( image_view(), real_defined_, context.subsample);

        // transform to linear colorspace here.
        OCIO::ConstConfigRcPtr config = application_t::Instance().current_ocio_config();
        OCIO::ConstProcessorRcPtr proc = config->getProcessor( get_value<std::string>( param( "colorspace")).c_str(), OCIO::ROLE_SCENE_LINEAR);
        image::ocio_transform( image_view(), proc);
    }
    catch( ...)
    {
    }
}

// factory
node_ptr_t create_image_input_node() { return new image_input_node_t();}

node_ptr_t create_gui_image_input_node()
{
    bool sequence, relative;
    boost::filesystem::path p;

    if( ui::user_interface_t::Instance().image_sequence_file_selector( p, sequence, relative))
        return new image_input_node_t( p, sequence);

    return 0;
}

const node_metaclass_t *image_input_node_t::metaclass() const { return &image_input_node_metaclass();}

const node_metaclass_t& image_input_node_t::image_input_node_metaclass()
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

static bool registered = node_factory_t::Instance().register_node( image_input_node_t::image_input_node_metaclass());

} // namespace
