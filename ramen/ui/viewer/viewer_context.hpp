// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_VIEWER_VIEWER_CONTEXT_HPP
#define	RAMEN_UI_VIEWER_VIEWER_CONTEXT_HPP

#include<ramen/ui/viewer/viewer_context_fwd.hpp>

#include<utility>

#include<ramen/GL/glew.hpp>

#include<QGLWidget>

#include<boost/shared_ptr.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

#include<OpenEXR/ImathColor.h>

#include<OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE;

#include<ramen/ocio/gl_lut3d.hpp>

#include<ramen/ui/viewer/viewer_strategy.hpp>

namespace ramen
{
namespace ui
{
namespace viewer
{

class viewer_context_t : public QGLWidget
{
    Q_OBJECT

public:

    enum view_mode_t
    {
        view_active_node = 0,
        view_context_node
    };

    viewer_context_t( QWidget *parent = 0);
    virtual ~viewer_context_t();

    // options
    view_mode_t view_mode() const       { return view_mode_;}
    void set_view_mode( view_mode_t m);

    bool autoupdate() const { return autoupdate_;}
    void set_autoupdate( bool b);

    // nodes
    void node_added( node_t *n);
    void node_released( node_t *n);

    void set_active_node( node_t *n);
    void set_context_node( node_t *n);

    // time
    void frame_changed();

    // ocio
    OCIO::ConstConfigRcPtr ocio_config() const;
    OCIO::DisplayTransformRcPtr ocio_transform();

    boost::shared_ptr<ocio::gl_lut3d_t>& display_lut();
    const std::string& display_device() const;
    const std::string& display_transform() const;
    float exposure() const;
    float gamma() const;

    void display_transform_changed();
    void exposure_changed();
    void gamma_changed();

    // util
    const Imath::Color3f& background_color() const { return back_color_;}
    const Imath::Color3f& foreground_color() const { return fg_color_;}

    void save_projection();
    void restore_projection();
    void set_screen_projection();

    void draw_checks_background() const;

    Imath::Color4f color_at( int x, int y) const;

    const viewer_strategy_t& strategy() const	{ return *current_;}
    viewer_strategy_t& strategy()				{ return *current_;}

protected:

    virtual void initializeGL();
    virtual void resizeGL( int w, int h);
    virtual void paintGL();

    virtual void enterEvent( QEvent *event);
    virtual void leaveEvent( QEvent *event);

    virtual void keyPressEvent( QKeyEvent *event);
    virtual void keyReleaseEvent( QKeyEvent *event);

    virtual void mouseMoveEvent( QMouseEvent *event);
    virtual void mousePressEvent( QMouseEvent *event);
    virtual void mouseReleaseEvent( QMouseEvent *event);

    virtual void wheelEvent( QWheelEvent *event);

private:

    typedef boost::ptr_vector<viewer_strategy_t>::reverse_iterator strategy_iterator;

    void test_gl_extensions();

    bool set_strategy_for_node( node_t *n);
    void set_strategy( strategy_iterator it);

    bool first_time_;

    boost::ptr_vector<viewer_strategy_t> strategies_;
    strategy_iterator current_;

    view_mode_t view_mode_;
    bool autoupdate_;

    boost::shared_ptr<ocio::gl_lut3d_t> display_lut_;

    Imath::Color3f back_color_, fg_color_;
};

} // viewer
} // ui
} // ramen

#endif
