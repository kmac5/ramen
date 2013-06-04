// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/image_input_param.hpp>

#include<algorithm>
#include<sstream>

#include<boost/filesystem/operations.hpp>

#include<adobe/algorithm/find.hpp>

#include<QPushButton>
#include<QVBoxLayout>
#include<QLabel>

#include<ramen/nodes/image/input/image_input_node.hpp>
#include<ramen/params/image_input_info.hpp>

#include<ramen/params/aspect_ratio_param.hpp>

#include<ramen/imageio/factory.hpp>
#include<ramen/imageio/multichannel_reader.hpp>

#include<ramen/app/document.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>

#include<ramen/ui/widgets/spinbox.hpp>
#include<ramen/ui/widgets/line_edit.hpp>

namespace ramen
{

image_input_param_t::image_input_param_t( const std::string& name) : static_param_t( name), start_frame_( 0), end_frame_( 0)
{
    value().assign( image_input_info_t());
}

image_input_param_t::image_input_param_t( const std::string& name, const boost::filesystem::path& p, bool sequence, float& aspect) : static_param_t( name)
{
    filesystem::path_sequence_t seq;

    if( sequence)
		seq = sequence_for_path( p);
    else
		seq = filesystem::path_sequence_t( p, false);

    boost::filesystem::path img_path( seq( seq.start()));

    try
    {
		std::auto_ptr<imageio::reader_t> reader = imageio::factory_t::Instance().reader_for_image( img_path);

		if( reader.get())
		{
			aspect = reader->aspect_ratio();
			imageio::multichannel_reader_t *mr = dynamic_cast<imageio::multichannel_reader_t*>( reader.get());

			if( mr && mr->has_extra_channels())
			{
				value().assign( image_input_info_t( seq, mr->channel_list()));
				return;
			}
		}
    }
    catch( std::exception& e)
    {
    }

    value().assign( image_input_info_t( seq));
	start_frame_ = seq.start();
	end_frame_ = seq.end();
}

image_input_param_t::image_input_param_t( const image_input_param_t& other) : static_param_t( other)
{
    input_ = 0;
    info_ = 0;
    red_combo_ = green_combo_ = blue_combo_ = alpha_combo_ = 0;
	start_frame_ = other.start_frame_;
	end_frame_ = other.end_frame_;
}

boost::filesystem::path image_input_param_t::path_for_frame( float f) const
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    return info( f);
}

boost::tuple<int,int,int,int> image_input_param_t::channels() const
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    return info.channels();
}

std::vector<std::string> image_input_param_t::channel_list() const
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    return info.channel_list();
}

void image_input_param_t::set_channels( const std::string& red, const std::string& green,
										const std::string& blue, const std::string& alpha)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    int c0, c1, c2, c3;

    std::vector<std::string>::const_iterator it;

    // red
    it = adobe::find( info.channel_list(), red);

    if( it != info.channel_list().end())
		c0 = std::distance( info.channel_list().begin(), it);
    else
		c0 = info.channel_list().size();

    // green
    it = adobe::find( info.channel_list(), green);

    if( it != info.channel_list().end())
		c1 = std::distance( info.channel_list().begin(), it);
    else
		c1 = info.channel_list().size();

    // blue
    it = adobe::find( info.channel_list(), blue);

    if( it != info.channel_list().end())
		c2 = std::distance( info.channel_list().begin(), it);
    else
		c2 = info.channel_list().size();

    // alpha
    it = adobe::find( info.channel_list(), alpha);

    if( it != info.channel_list().end())
		c3 = std::distance( info.channel_list().begin(), it);
    else
		c3 = info.channel_list().size();

    boost::tuple<int,int,int,int> ch( c0, c1, c2, c3);
    info.set_channels( ch);
    value().assign( info);
}

param_t *image_input_param_t::do_clone() const { return new image_input_param_t( *this);}

// paths
filesystem::path_sequence_t image_input_param_t::sequence_for_path( const boost::filesystem::path& p) const
{
    boost::filesystem::path dir;

    filesystem::path_sequence_t seq( p);

    if( p.is_relative())
		dir = document_t::Instance().composition().relative_to_absolute( p).parent_path();
    else
		dir = p.parent_path();

    for( boost::filesystem::directory_iterator itr(dir); itr != boost::filesystem::directory_iterator(); ++itr)
    {
		if( !boost::filesystem::is_directory( itr->path()))
			{
				if( itr->path().filename().string()[0] != '.')
					seq.add_path( itr->path());
			}
    }

    return seq;
}

// paths
void image_input_param_t::convert_relative_paths( const boost::filesystem::path& old_base, const boost::filesystem::path& new_base)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
	
	if( info.sequence().directory().is_relative())
	{
		boost::filesystem::path new_dir = filesystem::convert_relative_path( info.sequence().directory(), old_base, new_base);
		info.sequence().set_directory( new_dir);
	    value().assign( info);
		update_widgets();
	}
}

void image_input_param_t::make_paths_absolute()
{
	assert( composition());
	
    image_input_info_t info( get_value<image_input_info_t>( *this));
	
	if( info.sequence().directory().is_relative())
	{
		boost::filesystem::path new_dir = composition()->relative_to_absolute( info.sequence().directory());
		info.sequence().set_directory( new_dir);
	    value().assign( info);
		update_widgets();
	}
}

void image_input_param_t::make_paths_relative()
{
	assert( composition());
	
    image_input_info_t info( get_value<image_input_info_t>( *this));
	
	if( info.sequence().directory().is_absolute())
	{
		boost::filesystem::path new_dir = composition()->absolute_to_relative( info.sequence().directory());
		info.sequence().set_directory( new_dir);
	    value().assign( info);
		update_widgets();
	}
}

void image_input_param_t::do_add_to_hash( hash_generator_t& hash_gen) const
{
	// nothing to do here. The image input node 
	// takes care of doing it.
}

void image_input_param_t::do_read( const serialization::yaml_node_t& node)
{
	serialization::yaml_node_t vnode = node.get_node( "value");
	image_input_info_t val;
	val.read( vnode);
	value().assign( val);
}

void image_input_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
	image_input_info_t info( get_value<image_input_info_t>( *this));
	out << YAML::Key << "value" << YAML::Value;
	info.write( out);
}

// ui
QWidget *image_input_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    input_ = new ui::line_edit_t( top);
    QPushButton *button_ = new QPushButton( top);

    QSize s = input_->sizeHint();

    label->move( 0, 0);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());

    button_->move( ui::inspector_t::Instance().width() - s.height() - 10, 0);
    button_->resize( s.height(), s.height());
    button_->setText( "...");
    connect( button_, SIGNAL( clicked()), this, SLOT( select_pushed()));

    input_->move( ui::inspector_t::Instance().left_margin(), 0);
    input_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - button_->width() - 10, s.height());
    input_->setReadOnly( true);

    int current_height = s.height() + 5;

    label = new QLabel( top);
    label->move( 0, current_height);
    label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Info");

    info_ = new ui::line_edit_t( top);
    info_->move( ui::inspector_t::Instance().left_margin(), current_height);
    info_->setReadOnly( true);
    info_->resize( input_->width(), s.height());
    update_info();
    
    current_height += s.height() + 5;

    // popups
    {
		QLabel *label = new QLabel( top);
		red_combo_ = new QComboBox( top);
		red_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = red_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Red");

		red_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
		red_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
		connect( red_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    {
		QLabel *label = new QLabel( top);
		green_combo_ = new QComboBox( top);
		green_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = green_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Green");

		green_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
		green_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
		connect( green_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    {
		QLabel *label = new QLabel( top);
		blue_combo_ = new QComboBox( top);
		blue_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = blue_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Blue");

		blue_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
		blue_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
		connect( blue_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    {
		QLabel *label = new QLabel( top);
		alpha_combo_ = new QComboBox( top);
		alpha_combo_->setFocusPolicy( Qt::NoFocus);

		QSize s = alpha_combo_->sizeHint();

		label->move( 0, current_height);
		label->resize( ui::inspector_t::Instance().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Alpha");

		alpha_combo_->move( ui::inspector_t::Instance().left_margin(), current_height);
		alpha_combo_->resize( ui::inspector_t::Instance().width() - ui::inspector_t::Instance().left_margin() - 10, s.height());
		connect( alpha_combo_, SIGNAL( currentIndexChanged( int)), this, SLOT( channel_picked( int)));
		current_height += s.height() + 5;
    }

    top->setMinimumSize( ui::inspector_t::Instance().width(), current_height);
    top->setMaximumSize( ui::inspector_t::Instance().width(), current_height);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    do_update_widgets();
    return top;
}

void image_input_param_t::do_update_widgets()
{
	update_info();
	
    if( input_)
    {
		image_input_info_t info( get_value<image_input_info_t>( *this));
		input_->setText( QString::fromStdString( info.format_string()));
		
		std::vector<std::string> ch_list( channel_list());		
		update_popup( red_combo_, ch_list, boost::get<0>( info.channels()));
		update_popup( green_combo_, ch_list, boost::get<1>( info.channels()));
		update_popup( blue_combo_, ch_list, boost::get<2>( info.channels()));
		update_popup( alpha_combo_, ch_list, boost::get<3>( info.channels()));
    }
}

void image_input_param_t::select_pushed()
{
    boost::filesystem::path p;
    bool sequence, relative;

    if( ui::user_interface_t::Instance().image_sequence_file_selector( p, sequence, relative))
    {
        image_input_info_t info( get_value<image_input_info_t>( *this));
		filesystem::path_sequence_t seq;

		if( sequence)
			seq = sequence_for_path( p);
		else
			seq = filesystem::path_sequence_t( p, false);

		info.set_path_sequence( seq);

		boost::filesystem::path img_path( seq( seq.start()));
		std::auto_ptr<imageio::reader_t> reader;

		try
		{
			reader = imageio::factory_t::Instance().reader_for_image( img_path);
		}
		catch( ...)
		{
			
		}

		float aspect = 1.0f;
		aspect_ratio_param_t *asp_param = dynamic_cast<aspect_ratio_param_t*>( &( parameterised()->param( "aspect")));
		assert( asp_param);
				
		if( reader.get())
		{
			aspect = reader->aspect_ratio();
			
			imageio::multichannel_reader_t *mr = dynamic_cast<imageio::multichannel_reader_t*>( reader.get());
			if( mr && mr->has_extra_channels())
			{
				if( mr->channel_list() != channel_list())
					info.reset_channels();
	
				info.set_channel_list( mr->channel_list());
	
				param_set()->begin_edit();
				param_set()->add_command( this);
				value().assign( info);
				param_set()->param_changed( this);
				asp_param->set_value( aspect);
				param_set()->end_edit();
				asp_param->update_widgets();
				return;
			}
		}

		info.reset_channels();
		info.clear_channel_list();

		param_set()->begin_edit();
		param_set()->add_command( this);
		value().assign( info);
		param_set()->param_changed( this);
		asp_param->set_value( aspect);
		param_set()->end_edit();
		asp_param->update_widgets();
    }
}

void image_input_param_t::channel_picked( int index)
{
    image_input_info_t info( get_value<image_input_info_t>( *this));
    boost::tuple<int,int,int,int> ch( info.channels());

    if( QObject::sender() == red_combo_)
		boost::get<0>( ch) = index;

    if( QObject::sender() == green_combo_)
		boost::get<1>( ch) = index;

    if( QObject::sender() == blue_combo_)
		boost::get<2>( ch) = index;

    if( QObject::sender() == alpha_combo_)
		boost::get<3>( ch) = index;

    info.set_channels( ch);

    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    param_set()->param_changed( this);
    param_set()->end_edit();
}

void image_input_param_t::update_popup( QComboBox *popup, const std::vector<std::string>& ch_list, int value)
{
    popup->blockSignals( true);
    popup->clear();

    if( !ch_list.empty())
    {
		for( int i = 0; i < ch_list.size(); ++i)
			popup->addItem( ch_list[i].c_str());

		popup->addItem( "None");
		popup->setEnabled( true);
    }
    else
    {
		popup->addItem( "Red");
		popup->addItem( "Green");
		popup->addItem( "Blue");
		popup->addItem( "Alpha");
		popup->setEnabled( false);
    }

    popup->setCurrentIndex( value);
    popup->blockSignals( false);
}

void image_input_param_t::update_info()
{
	image_input_info_t seq_info( get_value<image_input_info_t>( *this));

	if( seq_info.valid())
	{
		start_frame_ = seq_info.sequence_start();
		end_frame_ = seq_info.sequence_end();
	}
	else
		start_frame_ = end_frame_ = 0;
	
    if( info_)
    {
		if( seq_info.valid())
		{
		    std::stringstream ss;
		    ss << "frames " << start_frame_ << "-" << end_frame_;

		    image_input_node_t *n = dynamic_cast<image_input_node_t*>( parameterised());
		    imageio::reader_t *r = n->reader();

		    if( r)
		    {
				Imath::Box2i format( r->format());
				ss << " size [" << format.size().x + 1 << " x " << format.size().y + 1 << "]";
		    }

		    info_->setText( ss.str().c_str());
		}
		else
		    info_->setText( "No Sequence");
    }
}

} // namespace
