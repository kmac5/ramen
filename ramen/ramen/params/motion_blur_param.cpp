// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/motion_blur_param.hpp>

#include<QComboBox>
#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/params/param_set.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/widgets/double_spinbox.hpp>

namespace ramen
{

motion_blur_param_t::motion_blur_param_t( const std::string& name) : static_param_t( name)
{
    value().assign( motion_blur_info_t());
    samples_ = 0;
    shutter_ = 0;
    shutter_off_ = 0;
    filter_ = 0;
}

motion_blur_param_t::motion_blur_param_t( const motion_blur_param_t& other) : static_param_t( other)
{
    samples_ = 0;
    shutter_ = 0;
    shutter_off_ = 0;
    filter_ = 0;
}

bool motion_blur_param_t::motion_blur_enabled() const
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));

    if( info.samples == 1 || info.shutter == 0)
        return false;

    return true;
}

motion_blur_info_t::loop_data_t motion_blur_param_t::loop_data( float time, int extra_samples, float shutter_factor) const
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    return motion_blur_info_t::loop_data_t( time, info.samples + extra_samples, info.shutter * shutter_factor,
                                            info.shutter_offset, info.filter);
}

void motion_blur_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    hash_gen << info.samples << "," << info.shutter << ","
            << info.shutter_offset << "," << info.filter;
}

void motion_blur_param_t::do_read( const serialization::yaml_node_t& node)
{
    motion_blur_info_t val;

    if( node.get_optional_value( "value", val))
        value().assign( val);
}

void motion_blur_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    out << YAML::Key << "value" << YAML::Value << info;
}

QWidget *motion_blur_param_t::do_create_widgets()
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));

    QWidget *top = new QWidget();

    shutter_ = new ui::double_spinbox_t( top);
    shutter_->setRange( 0, 1);
    shutter_->setSingleStep( 0.05);

    QSize s = shutter_->sizeHint();
    s.setWidth( s.width() + ( s.width() / 3));
    int h = 0;

    QLabel *label = new QLabel( top);
    label->move( 0, h);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Motion Blur Samples");

    samples_ = new ui::double_spinbox_t( top);
    samples_->setDecimals( 0);
    samples_->setTrackMouse( false);
    samples_->setRange( 1, 64);
    samples_->setValue( info.samples);
    samples_->move( app().ui()->inspector().left_margin(), h);
    samples_->resize( s.width(), s.height());
    samples_->setEnabled( enabled());
    connect( samples_, SIGNAL( valueChanged( double)), this, SLOT( samples_changed( double)));
    h += s.height() + 5;

    label = new QLabel( top);
    label->move( 0, h);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Shutter");

    shutter_->setTrackMouse( false);
    shutter_->setValue( info.shutter);
    shutter_->move( app().ui()->inspector().left_margin(), h);
    shutter_->resize( s.width(), s.height());
    shutter_->setEnabled( enabled());
    connect( shutter_, SIGNAL( valueChanged( double)), this, SLOT( shutter_changed( double)));
    h += s.height() + 5;

    label = new QLabel( top);
    label->move( 0, h);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Shutter Offset");

    shutter_off_ = new ui::double_spinbox_t( top);
    shutter_off_->setTrackMouse( false);
    shutter_off_->setRange( -1, 1);
    shutter_off_->setValue( info.shutter_offset);
    shutter_off_->setSingleStep( 0.05);
    shutter_off_->move( app().ui()->inspector().left_margin(), h);
    shutter_off_->resize( s.width(), s.height());
    shutter_off_->setEnabled( enabled());
    connect( shutter_off_, SIGNAL( valueChanged( double)), this, SLOT( shutter_changed( double)));
    h += s.height() + 5;

    label = new QLabel( top);
    label->move( 0, h);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( "Filter");

    filter_ = new QComboBox( top);
    s = filter_->sizeHint();
    filter_->setFocusPolicy( Qt::NoFocus);
    filter_->addItem( "Box");
    filter_->addItem( "Triangle");
    filter_->addItem( "Cubic");
    filter_->setCurrentIndex( (int) info.filter);
    filter_->move( app().ui()->inspector().left_margin(), h);
    filter_->resize( s.width(), s.height());
    filter_->setEnabled( enabled());
    connect( filter_, SIGNAL( currentIndexChanged( int)), this, SLOT( filter_changed( int)));
    h += s.height() + 5;

    top->setMinimumSize( app().ui()->inspector().width(), h);
    top->setMaximumSize( app().ui()->inspector().width(), h);
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void motion_blur_param_t::do_update_widgets()
{
    if( samples_)
    {
        motion_blur_info_t info( get_value<motion_blur_info_t>( *this));

        samples_->blockSignals( true);
        shutter_->blockSignals( true);
        shutter_off_->blockSignals( true);
        filter_->blockSignals( true);

        samples_->setValue( info.samples);
        shutter_->setValue( info.shutter);
        shutter_off_->setValue( info.shutter_offset);
        filter_->setCurrentIndex( (int) info.filter);

        samples_->blockSignals( false);
        shutter_->blockSignals( false);
        shutter_off_->blockSignals( false);
        filter_->blockSignals( false);
    }
}

void motion_blur_param_t::do_enable_widgets( bool e)
{
    if( samples_)
    {
        samples_->setEnabled( e);
        shutter_->setEnabled( e);
        shutter_off_->setEnabled( e);
        filter_->setEnabled( e);
    }
}

void motion_blur_param_t::samples_changed( double v)
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    info.samples = v;
    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    emit_param_changed( user_edited);
    param_set()->end_edit();
}

void motion_blur_param_t::shutter_changed( double v)
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    info.shutter = shutter_->value();
    info.shutter_offset = shutter_off_->value();
    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    emit_param_changed( user_edited);
    param_set()->end_edit();
}

void motion_blur_param_t::filter_changed( int index)
{
    motion_blur_info_t info( get_value<motion_blur_info_t>( *this));
    info.filter = (motion_blur_info_t::filter_type) index;
    param_set()->begin_edit();
    param_set()->add_command( this);
    value().assign( info);
    emit_param_changed( user_edited);
    param_set()->end_edit();
}

} // namespace
