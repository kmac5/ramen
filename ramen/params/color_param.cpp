// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/params/color_param.hpp>

#include<limits>
#include<cmath>

#include<boost/bind.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/assert.hpp>

#include<ramen/nodes/node.hpp>
#include<ramen/app/document.hpp>

#include<ramen/anim/track.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/widgets/param_spinbox.hpp>
#include<ramen/ui/widgets/color_button.hpp>
#include<ramen/ui/widgets/eyedropper_button.hpp>
#include<ramen/ui/widgets/color_picker.hpp>

namespace ramen
{

color_param_t::color_param_t( const std::string& name) : animated_param_t( name) { private_init();}

color_param_t::color_param_t( const color_param_t& other) : animated_param_t( other)
{
    is_rgba_ = other.is_rgba_;
    input0_ = input1_ = input2_ = input3_ = 0;
}

void color_param_t::private_init()
{
    set_is_rgba( true);

    add_curve( "R");
    add_curve( "G");
    add_curve( "B");
    add_curve( "A");

    set_min( 0);
    curve( 3).set_range( 0, 1); // <- Alpha channel

    set_default_value( Imath::Color4f( 0, 0, 0, 0));
    set_step( 0.025f);
}

void color_param_t::set_default_value( const Imath::Color4f& x)
{
    poly_param_indexable_value_t v( x);
    value() = adobe::poly_cast<poly_param_value_t&>( v);
}

poly_param_value_t color_param_t::value_at_frame(float frame) const
{
    Imath::Color4f v( get_value<Imath::Color4f>( *this));
    eval_curve( 0, frame, v.r);
    eval_curve( 1, frame, v.g);
    eval_curve( 2, frame, v.b);

    if( is_rgba())
    {
        eval_curve( 3, frame, v.a);
        v.a = adobe::clamp( v.a, 0.0f, 1.0f);
    }
    else
        v.a = 1.0f;

    poly_param_indexable_value_t val( v);
    return adobe::poly_cast<poly_param_value_t&>( val);
}

void color_param_t::set_value( const Imath::Color4f& x, change_reason reason)
{
    float frame = 1.0f;

    if( composition())
        frame = composition()->frame();

    set_value_at_frame( x, frame, reason);
}

void color_param_t::set_value_at_frame( const Imath::Color4f& x, float frame, change_reason reason)
{
    if( can_undo())
        param_set()->add_command( this);

    poly_param_indexable_value_t v( x);
    value() = adobe::poly_cast<poly_param_value_t&>( v);

    bool autokey = param_set()->autokey();

    if( !is_static() && ( autokey || !curve( 0).empty()))
        curve( 0).insert( frame, x.r);

    if( !is_static() && ( autokey || !curve( 1).empty()))
        curve( 1).insert( frame, x.g);

    if( !is_static() && ( autokey || !curve( 2).empty()))
        curve( 2).insert( frame, x.b);

    if( is_rgba() && !is_static() && ( autokey || !curve( 3).empty()))
        curve( 3).insert( frame, x.a);

    if( composition_t * c = composition())
        evaluate( c->frame());

    emit_param_changed( reason);
}

void color_param_t::do_create_tracks( anim::track_t *parent)
{
    std::auto_ptr<anim::track_t> t( new anim::track_t( name()));

    std::auto_ptr<anim::track_t> tr( new anim::track_t( "R", &( curve( 0))));
    tr->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tr);

    std::auto_ptr<anim::track_t> tg( new anim::track_t( "G", &( curve( 1))));
    tg->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tg);

    std::auto_ptr<anim::track_t> tb( new anim::track_t( "B", &( curve( 2))));
    tb->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
    t->add_child( tb);

    if( is_rgba())
    {
        std::auto_ptr<anim::track_t> ta( new anim::track_t( "A", &( curve( 3))));
        ta->changed.connect( boost::bind( &animated_param_t::anim_curve_changed, this, _1));
        t->add_child( ta);
    }

    parent->add_child( t);
}

void color_param_t::do_add_to_hash( hash::generator_t& hash_gen) const
{
    Imath::Color4f c = get_value<Imath::Color4f>( *this);
    hash_gen << c.r <<"," << c.g << "," << c.b;

    if( is_rgba())
        hash_gen << "," << c.a;
}

void color_param_t::do_read( const serialization::yaml_node_t& node)
{
    read_curves( node);

    Imath::Color4f val;
    if( node.get_optional_value( "value", val))
    {
        poly_param_indexable_value_t v( val);
        value().assign( adobe::poly_cast<poly_param_value_t&>( v));
    }
}

void color_param_t::do_write( serialization::yaml_oarchive_t& out) const
{
    write_curves( out);
    bool one   = curve( 0).empty(); // && expression( 0).empty()
    bool two   = curve( 1).empty(); // && expression( 1).empty()
    bool three = curve( 2).empty(); // && expression( 2).empty()
    bool four  = curve( 3).empty() && is_rgba();  // && expression( 3).empty()

    if( one || two || three || four)
        out << YAML::Key << "value" << YAML::Value << get_value<Imath::Color4f>( *this);
}

void color_param_t::do_update_widgets()
{
    if( input0_)
    {
        input0_->blockSignals( true);
        input1_->blockSignals( true);
        input2_->blockSignals( true);

        if( input3_)
            input3_->blockSignals( true);

        button_->blockSignals( true);

        Imath::Color4f col = get_value<Imath::Color4f>( *this);
        input0_->setValue( col.r);
        input1_->setValue( col.g);
        input2_->setValue( col.b);

        if( input3_)
            input3_->setValue( col.a);

        ui::color_t c( col.r, col.g, col.b, col.a);
        c.apply_gamma( 1.0 / 2.2);
        button_->set_value( c);

        input0_->blockSignals( false);
        input1_->blockSignals( false);
        input2_->blockSignals( false);

        if( input3_)
            input3_->blockSignals( false);

        button_->blockSignals( false);
    }
}

void color_param_t::do_enable_widgets( bool e)
{
    if( input0_)
    {
        input0_->setEnabled( e);
        input1_->setEnabled( e);
        input2_->setEnabled( e);

        if( input3_)
            input3_->setEnabled( e);

        button_->setEnabled( e);
        eyedropper_->setEnabled( e);
    }
}

QWidget *color_param_t::do_create_widgets()
{
    QWidget *top = new QWidget();
    QLabel *label = new QLabel( top);
    button_ = new ui::color_button_t( top);

    input0_ = new ui::param_spinbox_t( *this, 0, top);
    input1_ = new ui::param_spinbox_t( *this, 1, top);
    input2_ = new ui::param_spinbox_t( *this, 2, top);

    if( is_rgba())
        input3_ = new ui::param_spinbox_t(*this, 3, top);

    QSize s = input0_->sizeHint();

    label->move( 0, 0);
    label->resize( app().ui()->inspector().left_margin() - 5, s.height());
    label->setAlignment( Qt::AlignRight | Qt::AlignVCenter);
    label->setText( name().c_str());
    label->setToolTip( id().c_str());
    Imath::Color4f col = get_value<Imath::Color4f>( *this);

    int xpos = app().ui()->inspector().left_margin();

    button_->move( xpos, 0);
    button_->resize( s.height(), s.height());
    button_->set_value( ui::color_t( std::pow( (double) col.r, 1.0 / 2.2),
                                      std::pow( (double) col.g, 1.0 / 2.2),
                                        std::pow( (double) col.b, 1.0 / 2.2)));

    button_->setEnabled( enabled());
    connect( button_, SIGNAL( pressed()), this, SLOT( color_button_pressed()));
    xpos += s.height();

    eyedropper_ = new ui::eyedropper_button_t( top);
    eyedropper_->move( xpos, 0);
    eyedropper_->resize( s.height(), s.height());
    eyedropper_->setEnabled( enabled());
    connect( eyedropper_, SIGNAL( color_picked( const ramen::ui::color_t&)), this, SLOT( eyedropper_color_picked( const ramen::ui::color_t&)));
    xpos += s.height() + 3;

    // make spinboxes a bit smaller
    s.setWidth( s.width() - 10);

    input0_->move( xpos, 0);
    input0_->resize( s.width(), s.height());
    input0_->setMinimum( 0);
    input0_->setDecimals( 4);
    input0_->setValue( col.r);
    input0_->setSingleStep( step());
    input0_->setEnabled( enabled());
    connect( input0_, SIGNAL( valueChanged(double)), button_, SLOT( set_red(double)));
    connect( input0_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input0_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input0_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input0_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += s.width() + 3;

    input1_->move( xpos, 0);
    input1_->resize( s.width(), s.height());
    input1_->setMinimum( 0);
    input1_->setDecimals( 4);
    input1_->setValue( col.g);
    input1_->setSingleStep( step());
    input1_->setEnabled( enabled());
    connect( input1_, SIGNAL( valueChanged(double)), button_, SLOT( set_green(double)));
    connect( input1_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input1_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input1_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input1_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += s.width() + 3;

    input2_->move( xpos, 0);
    input2_->resize( s.width(), s.height());
    input2_->setMinimum( 0);
    input2_->setDecimals( 4);
    input2_->setValue( col.b);
    input2_->setSingleStep( step());
    input2_->setEnabled( enabled());
    connect( input2_, SIGNAL( valueChanged(double)), button_, SLOT( set_blue(double)));
    connect( input2_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
    connect( input2_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
    connect( input2_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
    connect( input2_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
    xpos += s.width() + 3;

    if( is_rgba())
    {
        input3_->move( xpos, 0);
        input3_->resize( s.width(), s.height());
        input3_->setValue( col.a);
        input3_->setRange( 0, 1);
        input3_->setSingleStep( step());
        input3_->setEnabled( enabled());
        connect( input3_, SIGNAL( valueChanged( double)), this, SLOT( value_changed( double)));
        connect( input3_, SIGNAL( spinBoxPressed()), this, SLOT( spinbox_pressed()));
        connect( input3_, SIGNAL( spinBoxDragged( double)), this, SLOT( spinbox_dragged( double)));
        connect( input3_, SIGNAL( spinBoxReleased()), this, SLOT( spinbox_released()));
        xpos += s.width() + 3;
    }

    top->setMinimumSize( app().ui()->inspector().width(), s.height());
    top->setMaximumSize( app().ui()->inspector().width(), s.height());
    top->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    return top;
}

void color_param_t::set_component_value_from_slot()
{
    Imath::Color4f v( input0_->value(), input1_->value(), input2_->value(), 1.0f);

    if( is_rgba())
        v.a = input3_->value();

    int index;
    float comp_value;

    if( QObject::sender() == input0_)
    {
        index = 0;
        comp_value = v.r;
    }
    else
    {
        if( QObject::sender() == input1_)
        {
            index = 1;
            comp_value = v.g;
        }
        else
        {
            if( QObject::sender() == input2_)
            {
                index = 2;
                comp_value = v.b;
            }
            else
            {
                RAMEN_ASSERT( is_rgba());

                index = 3;
                comp_value = v.a;
            }
        }
    }

    set_component_value( index, comp_value);
}

void color_param_t::value_changed( double value)
{
    param_set()->begin_edit();
    set_component_value_from_slot();
    param_set()->end_edit();
}

void color_param_t::spinbox_pressed()
{
    if( track_mouse())
        app().ui()->begin_interaction();

    param_set()->begin_edit();
}

void color_param_t::spinbox_dragged( double value)
{
    set_component_value_from_slot();

    if( track_mouse())
        param_set()->notify_parent();

    app().ui()->update_anim_editors();
}

void color_param_t::spinbox_released()
{
    param_set()->end_edit( !track_mouse());

    if( track_mouse())
        app().ui()->end_interaction();
}

void color_param_t::eyedropper_color_picked( const ramen::ui::color_t& c)
{
    param_set()->begin_edit();

    if( is_rgba())
        set_value( Imath::Color4f( c.red(), c.green(), c.blue(), c.alpha()));
    else
        set_value( Imath::Color4f( c.red(), c.green(), c.blue(), 1));

    update_widgets();
    param_set()->end_edit();
}

void color_param_t::color_button_pressed()
{
    Imath::Color4f col = get_value<Imath::Color4f>( *this);
    ui::color_t c( col.r, col.g, col.b);
    ui::color_picker_t *picker = new ui::color_picker_t( app().ui()->main_window(), c);

    if( picker->exec() == QDialog::Accepted)
    {
        c = picker->color();

        param_set()->begin_edit();

        if( is_rgba())
            set_value( Imath::Color4f( c.red(), c.green(), c.blue(), col.a));
        else
            set_value( Imath::Color4f( c.red(), c.green(), c.blue(), 1));

        update_widgets();
        param_set()->end_edit();
    }

    delete picker;
}

} // namespace
