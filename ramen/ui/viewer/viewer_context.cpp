// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/viewer/viewer_context.hpp>

#include<boost/bind.hpp>
#include<boost/range/algorithm/for_each.hpp>

#include<ramen/GL/glu.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/viewer/image_view/image_viewer_strategy.hpp>

#include<iostream>

namespace ramen
{
namespace ui
{
namespace viewer
{

viewer_context_t::viewer_context_t( QWidget *parent) : QGLWidget( parent)
{
    first_time_ = true;
    autoupdate_ = true;
    view_mode_ = view_active_node;

    setFocusPolicy( Qt::ClickFocus);
    setMouseTracking( true);

    view_mode_ = view_active_node;

    // add the default viewer strategy, always first
    strategies_.push_back( new default_viewer_strategy_t( this));

    // add other viewer strategies
    strategies_.push_back( new image_viewer_strategy_t( this));
    // ...

    // set the default, current
    current_ = strategies_.rend() - 1;

    // get the palette colors
    QColor col( palette().color( QPalette::Base));
    back_color_ = Imath::Color3f( col.red() / 255.0f,
                                  col.green() / 255.0f,
                                  col.blue() / 255.0f);

    col = palette().color( QPalette::ButtonText);
    fg_color_ = Imath::Color3f( col.red() / 255.0f,
                                col.green() / 255.0f,
                                col.blue() / 255.0f);
}

viewer_context_t::~viewer_context_t()
{
    strategy().end_active_view();
}

void viewer_context_t::set_view_mode( view_mode_t m)
{ 
    if( m != view_mode_)
    {
        view_mode_ = m;

        if( view_mode_ == view_active_node)
            set_strategy_for_node( app().ui()->active_node());
        else
            set_strategy_for_node( app().ui()->context_node());

        strategy().view_mode_changed();
        update();
    }
}

void viewer_context_t::set_autoupdate( bool b)
{
    if( autoupdate_ != b)
    {
        autoupdate_ = b;
        strategy().autoupdate_changed();
    }
}

void viewer_context_t::node_added( node_t *n)
{
    boost::range::for_each( strategies_,
                            boost::bind( &viewer_strategy_t::node_added, _1, n));
}

void viewer_context_t::node_released( node_t *n)
{
    boost::range::for_each( strategies_,
                            boost::bind( &viewer_strategy_t::node_released, _1, n));
}

void viewer_context_t::set_active_node( node_t *n)
{
    if( view_mode() == view_active_node)
        set_strategy_for_node( n);

    strategy_iterator it( strategies_.rbegin());

    for( ; it != strategies_.rend(); ++it)
    {
        if( it->can_display_node( n))
            it->set_active_node( n, ( it == current_ &&  view_mode() == view_active_node));
        else
            it->set_active_node( 0);
    }

    if( view_mode() == view_active_node)
        update();
}

void viewer_context_t::set_context_node( node_t *n)
{
    if( view_mode() == view_context_node)
        set_strategy_for_node( n);

    strategy_iterator it( strategies_.rbegin());

    for( ; it != strategies_.rend(); ++it)
    {
        if( it->can_display_node( n))
            it->set_context_node( n, ( it == current_ &&  view_mode() == view_context_node));
        else
            it->set_context_node( 0);
    }

    if( view_mode() == view_context_node)
        update();
}

bool viewer_context_t::set_strategy_for_node( node_t *n)
{
    strategy_iterator it( strategies_.rbegin());

    for( ; it != strategies_.rend(); ++it)
    {
        if( it->can_display_node( n))
            break;
    }

    if( it != current_)
    {
        set_strategy( it);
        return true;
    }

    return false;
}

void viewer_context_t::set_strategy( strategy_iterator it)
{
    strategy().end_active_view();
    it->begin_active_view();
    app().ui()->viewer().set_viewer_toolbar( it->toolbar());
    current_ = it;
}

void viewer_context_t::frame_changed() { strategy().frame_changed();}

// ocio
OCIO::ConstConfigRcPtr viewer_context_t::ocio_config() const
{
    return app().ocio_manager().config();
}

OCIO::DisplayTransformRcPtr viewer_context_t::ocio_transform()
{
    OCIO::DisplayTransformRcPtr transform = OCIO::DisplayTransform::Create();
    transform->setInputColorSpaceName( OCIO::ROLE_SCENE_LINEAR);
    transform->setDisplay( display_device().c_str());
    transform->setView( display_transform().c_str());
    return transform;
}

boost::shared_ptr<ocio::gl_lut3d_t>& viewer_context_t::display_lut()
{
    return display_lut_;
}

const std::string& viewer_context_t::display_device() const
{
    return app().ui()->viewer().display_device();
}

const std::string& viewer_context_t::display_transform() const
{
    return app().ui()->viewer().display_transform();
}

float viewer_context_t::exposure() const
{
    return app().ui()->viewer().exposure();
}

float viewer_context_t::gamma() const
{
    return app().ui()->viewer().gamma();
}

void viewer_context_t::display_transform_changed()
{
    strategy().display_transform_changed();
}

void viewer_context_t::exposure_changed()
{
    strategy().exposure_changed();
}

void viewer_context_t::gamma_changed()
{
    strategy().gamma_changed();
}

// QGLWidget
void viewer_context_t::initializeGL()
{
    clear_gl_errors();

    gl_clear_color( back_color_.x, back_color_.y, back_color_.z, 0);

    if( first_time_)
    {
        test_gl_extensions();
        display_lut_.reset( new ocio::gl_lut3d_t());
        boost::range::for_each( strategies_,
                                boost::bind( &viewer_strategy_t::init, _1));
        strategy().begin_active_view();
        first_time_ = false;
    }
    else
    {
        std::cout << "GL context destroyed!\n";
        // add a call to reinitialize the context?
    }
}

void viewer_context_t::resizeGL( int w, int h)
{
    boost::range::for_each( strategies_,
                            boost::bind( &viewer_strategy_t::resize, _1, w, h));
}

void viewer_context_t::paintGL() { strategy().paint();}

void viewer_context_t::enterEvent( QEvent *event)
{
    strategy().enter_event( event);
}

void viewer_context_t::leaveEvent( QEvent *event)
{
    strategy().leave_event( event);
}

void viewer_context_t::keyPressEvent( QKeyEvent *event)
{
    strategy().key_press_event( event);
}

void viewer_context_t::keyReleaseEvent( QKeyEvent *event)
{
    strategy().key_release_event( event);
}

void viewer_context_t::mousePressEvent( QMouseEvent *event)
{
    strategy().mouse_press_event( event);
}

void viewer_context_t::mouseMoveEvent( QMouseEvent *event)
{
    strategy().mouse_move_event( event);
}

void viewer_context_t::mouseReleaseEvent( QMouseEvent *event)
{
    strategy().mouse_release_event( event);
}

void viewer_context_t::test_gl_extensions()
{
    GLenum err = glewInit();

    if( GLEW_OK != err)
        app().fatal_error( "Video card not supported: Error initializing GLEW");

    // test common extensions here
    if( !glewIsSupported("GL_VERSION_2_0"))
        app().fatal_error( "Video card not supported: Not OpenGL 2.0");

    if( !GLEW_ARB_texture_non_power_of_two)
        app().fatal_error( "Video card not supported: Not non power of two textures");

    if( !GLEW_ARB_texture_float)
        app().fatal_error( "Video card not supported: Not float textures");

    if( !GLEW_ARB_fragment_shader)
        app().fatal_error( "Video card not supported: Not fragment programs");

    // OCIO GPU path support
    if( !GLEW_EXT_texture3D)
        app().fatal_error( "Video card not supported: No 3D textures");

    if( !GLEW_ARB_multitexture)
        app().fatal_error( "Video card not supported: No multitexture");

    GLint tx_units;
    glGetIntegerv( GL_MAX_TEXTURE_UNITS, &tx_units);
    if( tx_units < 2)
        app().fatal_error( "Video card not supported: Not enough texture units");

    // let each implementation test other extensions
    boost::range::for_each( strategies_,
                            boost::bind( &viewer_strategy_t::test_gl_extensions, _1));
}

// utils
void viewer_context_t::save_projection()
{
    gl_matrix_mode( GL_PROJECTION);
    gl_push_matrix();
    gl_matrix_mode( GL_MODELVIEW);
    gl_push_matrix();
}

void viewer_context_t::restore_projection()
{
    gl_matrix_mode( GL_PROJECTION);
    gl_pop_matrix();
    gl_matrix_mode( GL_MODELVIEW);
    gl_pop_matrix();
}

void viewer_context_t::set_screen_projection()
{
    gl_matrix_mode( GL_PROJECTION);
    gl_load_identity();
    gl_viewport( 0, 0, width(), height());
    glu_ortho2d( 0, width(), height(), 0);
    gl_matrix_mode( GL_MODELVIEW);
}

void viewer_context_t::draw_checks_background() const
{
    // TODO: this is better done with a texture.
    const int checks_size = 12;

    for( int j = 0; j < height(); j += checks_size)
    {
        for( int i = 0; i < width(); i += checks_size)
        {
            if( i / checks_size % 2)
            {
                if( !( j / checks_size % 2))
                    gl_color3f( .6, .6, .6);
                else
                    gl_color3f( .8, .8, .8);
            }
            else
            {
                if( !( j / checks_size % 2))
                    gl_color3f( .8, .8, .8);
                else
                    gl_color3f( .6, .6, .6);
            }

            gl_begin( GL_QUADS);
            gl_vertex2i( i, j);
            gl_vertex2i( i+checks_size, j);
            gl_vertex2i( i+checks_size, j+checks_size);
            gl_vertex2i( i, j+checks_size);
            gl_end();
        }
    }
}

Imath::Color4f viewer_context_t::color_at( int x, int y) const
{
    return strategy().color_at( x, y);
}

} // viewer
} // ui
} // ramen

/*
std::pair<std::string, std::string>  viewer_context_t::get_context_pair( int index)
{
    return app().document().composition().ocio_context_pairs()[ index];
}
*/
