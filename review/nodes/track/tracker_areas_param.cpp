// Copyright (c) 2011 Esteban Tovagliari

#include<ramen/params/tracker_areas_param.hpp>

#include<QComboBox>
#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>

#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/double_spinbox.hpp>

namespace ramen
{

float tracker_areas_t::min_reference_radius()	{ return 3;}
float tracker_areas_t::min_search_radius()		{ return 5;}
	
tracker_areas_t::tracker_areas_t( int r, int s)
{
	set_areas( Imath::Box2f( Imath::V2f( r, r), Imath::V2f( r, r)),
				Imath::Box2f( Imath::V2f( s, s), Imath::V2f( s, s)));
}
		
tracker_areas_t::tracker_areas_t( const Imath::Box2f& ref, const Imath::Box2f& srch) { set_areas( ref, srch);}
		
void tracker_areas_t::set_areas( const Imath::Box2f& r, const Imath::Box2f& s)
{
	reference.min.x = std::max( r.min.x, min_reference_radius());
	reference.min.y = std::max( r.min.y, min_reference_radius());
	reference.max.x = std::max( r.max.x, min_reference_radius());
	reference.max.y = std::max( r.max.y, min_reference_radius());

	search.min.x = std::max( std::max( s.min.x, reference.min.x + 2), min_search_radius());
	search.min.y = std::max( std::max( s.min.y, reference.min.y + 2), min_search_radius());
	search.max.x = std::max( std::max( s.max.x, reference.max.x + 2), min_search_radius());
	search.max.y = std::max( std::max( s.max.y, reference.max.y + 2), min_search_radius());
}

void tracker_areas_t::set_reference_area( const Imath::Box2f& r)	{ set_areas( r, search);}
void tracker_areas_t::set_search_area( const Imath::Box2f& s)		{ set_areas( reference, s);}

bool tracker_areas_t::operator==( const tracker_areas_t& other) const
{
	return reference == other.reference && search == other.search;
}

bool tracker_areas_t::operator!=( const tracker_areas_t& other) const
{
	return reference != other.reference || search != other.search;
}

void operator>>( const YAML::Node& in, tracker_areas_t& areas)
{
	in[0] >> areas.reference.min.x;
	in[1] >> areas.reference.min.y;
	in[2] >> areas.reference.max.x;
	in[3] >> areas.reference.max.y;
	in[4] >> areas.search.min.x;
	in[5] >> areas.search.min.y;
	in[6] >> areas.search.max.x;
	in[7] >> areas.search.max.y;
}

YAML::Emitter& operator<<( YAML::Emitter& out, const tracker_areas_t& areas)
{
	out << YAML::Flow << YAML::BeginSeq
		<< ( int) areas.reference.min.x
		<< ( int) areas.reference.min.y
		<< ( int) areas.reference.max.x
		<< ( int) areas.reference.max.y
		<< ( int) areas.search.min.x
		<< ( int) areas.search.min.y
		<< ( int) areas.search.max.x
		<< ( int) areas.search.max.y
		<< YAML::EndSeq;
	return out;
}
	
tracker_areas_param_t::tracker_areas_param_t() : static_param_t( "Tracker Areas")
{
	set_track_mouse( false);
    value().assign( tracker_areas_t( 15, 30));
	
	ref_minx_ = 0;
	ref_miny_ = 0;
	ref_maxx_ = 0;
	ref_maxy_ = 0;
	search_minx_ = 0;
	search_miny_ = 0;
	search_maxx_ = 0;
	search_maxy_ = 0;
}

tracker_areas_param_t::tracker_areas_param_t( const tracker_areas_param_t& other) : static_param_t( other)
{
	ref_minx_ = 0;
	ref_miny_ = 0;
	ref_maxx_ = 0;
	ref_maxy_ = 0;
	search_minx_ = 0;
	search_miny_ = 0;
	search_maxx_ = 0;
	search_maxy_ = 0;
}

void tracker_areas_param_t::set_value( const tracker_areas_t& areas)
{
    if( can_undo())
		param_set()->add_command( this);

    value().assign( areas);
	emit_param_changed( user_edited);
}

void tracker_areas_param_t::do_read( const serialization::yaml_node_t& node)
{
	tracker_areas_t val;
	if( node.get_optional_value( "value", val))
		value().assign( val);	
}

void tracker_areas_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    tracker_areas_t val( get_value<tracker_areas_t>( *this));
    out << YAML::Key << "value" << YAML::Value << val;
}

QWidget *tracker_areas_param_t::do_create_widgets()
{
    tracker_areas_t areas( get_value<tracker_areas_t>( *this));
	
    QWidget *top = new QWidget();

    ref_minx_ = new ui::double_spinbox_t( top);
	ref_miny_ = new ui::double_spinbox_t( top);
	ref_maxx_ = new ui::double_spinbox_t( top);
	ref_maxy_ = new ui::double_spinbox_t( top);

    search_minx_ = new ui::double_spinbox_t( top);
	search_miny_ = new ui::double_spinbox_t( top);
	search_maxx_ = new ui::double_spinbox_t( top);
	search_maxy_ = new ui::double_spinbox_t( top);
		
    QSize s = ref_minx_->sizeHint();
    s.setWidth( s.width() - 10);
	
    int h = 0;

	// reference
	{
		int xpos = app().ui()->inspector().left_margin();
		
		QLabel *label = new QLabel( top);
		label->move( 0, h);
		label->resize( app().ui()->inspector().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Reference");
	
		ref_minx_->setDecimals( 0);
		ref_minx_->setMinimum( tracker_areas_t::min_reference_radius());
		ref_minx_->setValue( ( int) areas.reference.min.x);
		ref_minx_->move( xpos, h);
		ref_minx_->resize( s.width(), s.height());
		ref_minx_->setEnabled( enabled());
		connect( ref_minx_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		xpos += s.width() + 3;
		
		ref_miny_->setDecimals( 0);
		ref_miny_->setMinimum( tracker_areas_t::min_reference_radius());
		ref_miny_->setValue( ( int) areas.reference.min.y);
		ref_miny_->move( xpos, h);
		ref_miny_->resize( s.width(), s.height());
		ref_miny_->setEnabled( enabled());
		connect( ref_miny_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		xpos += s.width() + 3;
	
		ref_maxx_->setDecimals( 0);
		ref_maxx_->setMinimum( tracker_areas_t::min_reference_radius());
		ref_maxx_->setValue( ( int) areas.reference.max.x);
		ref_maxx_->move( xpos, h);
		ref_maxx_->resize( s.width(), s.height());
		ref_maxx_->setEnabled( enabled());
		connect( ref_maxx_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		xpos += s.width() + 3;
		
		ref_maxy_->setDecimals( 0);
		ref_maxy_->setMinimum( tracker_areas_t::min_reference_radius());
		ref_maxy_->setValue( ( int) areas.reference.max.y);
		ref_maxy_->move( xpos, h);
		ref_maxy_->resize( s.width(), s.height());
		ref_maxy_->setEnabled( enabled());
		connect( ref_maxy_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		h += s.height() + 5;
	}
	
	// search
	{
		int xpos = app().ui()->inspector().left_margin();
		
		QLabel *label = new QLabel( top);
		label->move( 0, h);
		label->resize( app().ui()->inspector().left_margin() - 5, s.height());
		label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
		label->setText( "Search");
	
		search_minx_->setDecimals( 0);
		search_minx_->setMinimum( tracker_areas_t::min_search_radius());
		search_minx_->setValue( ( int) areas.search.min.x);
		search_minx_->move( xpos, h);
		search_minx_->resize( s.width(), s.height());
		search_minx_->setEnabled( enabled());
		connect( search_minx_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		xpos += s.width() + 3;
		
		search_miny_->setDecimals( 0);
		search_miny_->setMinimum( tracker_areas_t::min_search_radius());
		search_miny_->setValue( ( int) areas.search.min.y);
		search_miny_->move( xpos, h);
		search_miny_->resize( s.width(), s.height());
		search_miny_->setEnabled( enabled());
		connect( search_miny_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		xpos += s.width() + 3;
	
		search_maxx_->setDecimals( 0);
		search_maxx_->setMinimum( tracker_areas_t::min_search_radius());
		search_maxx_->setValue( ( int) areas.search.max.x);
		search_maxx_->move( xpos, h);
		search_maxx_->resize( s.width(), s.height());
		search_maxx_->setEnabled( enabled());
		connect( search_maxx_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		xpos += s.width() + 3;
		
		search_maxy_->setDecimals( 0);
		search_maxy_->setMinimum( tracker_areas_t::min_search_radius());
		search_maxy_->setValue( ( int) areas.search.max.y);
		search_maxy_->move( xpos, h);
		search_maxy_->resize( s.width(), s.height());
		search_maxy_->setEnabled( enabled());
		connect( search_maxy_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
		h += s.height() + 5;
	}	

    top->setMinimumSize( app().ui()->inspector().width(), h);
    top->setMaximumSize( app().ui()->inspector().width(), h);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void tracker_areas_param_t::do_update_widgets()
{
    if( ref_minx_)
    {
		tracker_areas_t areas( get_value<tracker_areas_t>( *this));
		
		ref_minx_->blockSignals( true);
		ref_miny_->blockSignals( true);
		ref_maxx_->blockSignals( true);
		ref_maxy_->blockSignals( true);
		search_minx_->blockSignals( true);
		search_miny_->blockSignals( true);
		search_maxx_->blockSignals( true);
		search_maxy_->blockSignals( true);

		ref_minx_->setValue( ( int) areas.reference.min.x);
		ref_miny_->setValue( ( int) areas.reference.min.y);
		ref_maxx_->setValue( ( int) areas.reference.max.x);
		ref_maxy_->setValue( ( int) areas.reference.max.y);
		search_minx_->setValue( ( int) areas.search.min.x);
		search_miny_->setValue( ( int) areas.search.min.y);
		search_maxx_->setValue( ( int) areas.search.max.x);
		search_maxy_->setValue( ( int) areas.search.max.y);

		ref_minx_->blockSignals( false);
		ref_miny_->blockSignals( false);
		ref_maxx_->blockSignals( false);
		ref_maxy_->blockSignals( false);
		search_minx_->blockSignals( false);
		search_miny_->blockSignals( false);
		search_maxx_->blockSignals( false);
		search_maxy_->blockSignals( false);		
    }
}

void tracker_areas_param_t::do_enable_widgets( bool e)
{
    if( ref_minx_)
    {
		ref_minx_->setEnabled( e);
		ref_miny_->setEnabled( e);
		ref_maxx_->setEnabled( e);
		ref_maxy_->setEnabled( e);
		search_minx_->setEnabled( e);
		search_miny_->setEnabled( e);
		search_maxx_->setEnabled( e);
		search_maxy_->setEnabled( e);
    }
}

void tracker_areas_param_t::value_changed( double v)
{
	tracker_areas_t areas( Imath::Box2f( Imath::V2f( ref_minx_->value(), ref_miny_->value()),
										 Imath::V2f( ref_maxx_->value(), ref_maxy_->value())),
						   Imath::Box2f( Imath::V2f( search_minx_->value(), search_miny_->value()),
										 Imath::V2f( search_maxx_->value(), search_maxy_->value())));
    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( areas);
	emit_param_changed( user_edited);
    param_set()->end_edit();
	update_widgets();
}

} // namespace
