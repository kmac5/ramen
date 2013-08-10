// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_HPP
#define RAMEN_UI_HPP

#include<QObject>
#include<QString>
#include<QFont>

// avoid a moc error, when including boost
#ifndef QT_MOC_RUN
    #include<ramen/config.hpp>

    #include<ramen/ui/user_interface_fwd.hpp>

    #include<memory>

    #include<boost/noncopyable.hpp>
    #include<boost/thread/future.hpp>

    #include<ramen/filesystem/path.hpp>
    #include<boost/filesystem/fstream.hpp>

    #include<ramen/render/image_node_renderer.hpp>

    #include<ramen/ui/main_window_fwd.hpp>
    #include<ramen/ui/viewer/viewer_fwd.hpp>
    #include<ramen/ui/inspector/inspector_fwd.hpp>
    #include<ramen/ui/anim/anim_editor_fwd.hpp>

    #include<ramen/serialization/archive_fwd.hpp>
#endif

namespace ramen
{
namespace ui
{

/**
\ingroup ui
\brief user interface class.
*/
class RAMEN_API user_interface_t : public QObject, boost::noncopyable
{
    Q_OBJECT

public:

    user_interface_t();
    ~user_interface_t();

    void init();

    void show();
    int run( const boost::filesystem::path& p = boost::filesystem::path());
    void quit();

    // components
    const main_window_t *main_window() const    { return window_;}
    main_window_t *main_window()                { return window_;}

    const inspector_t& inspector() const    { return *inspector_;}
    inspector_t& inspector()                { return *inspector_;}

    const anim_editor_t& anim_editor() const { return *anim_editor_;}
    anim_editor_t& anim_editor()             { return *anim_editor_;}

    const viewer_t& viewer() const  { return *viewer_;}
    viewer_t& viewer()              { return *viewer_;}

    // document handling
    void create_new_document();
    void open_document( const boost::filesystem::path& p);
    bool save_document();

    // nodes
    void node_added( node_t *n);
    void node_released( node_t *n);

    void begin_interaction();
    void end_interaction();
    bool interacting() const { return interacting_;}

    node_t *active_node() const { return active_;}
    void set_active_node( node_t *n);

    node_t *context_node() const { return context_;}
    void set_context_node( node_t *n);

    void update();

    // time
    int start_frame() const;
    int end_frame() const;
    float frame() const;

    // anim
    void update_anim_editors();

    // error reporting
    void fatal_error( const std::string& msg) const;
    void error( const std::string& msg) const;
    void inform( const std::string& msg) const;
    bool question( const std::string& what, bool default_answer = true) const;

    // sequence file selector
    bool image_sequence_file_selector( boost::filesystem::path& p, bool& sequence, bool& relative) const;
    bool image_sequence_file_selector( const std::string& title, const std::string& types, boost::filesystem::path& p,
                                        bool& sequence, bool& relative) const;

    // serialization
    void read_ui_state( const serialization::yaml_iarchive_t& in);
    void write_ui_state( serialization::yaml_oarchive_t& out) const;

    // event filtering
    void start_long_process();
    void process_events();
    void end_long_process();
    bool process_cancelled() const;

    virtual bool eventFilter( QObject *watched, QEvent *event);

    // render
    boost::unique_future<bool>& render_image( render::context_t context, render::image_node_renderer_t& renderer);

    bool rendering() const { return rendering_;}

    // util
    int viewer_toolbar_height() const;

    const QString& image_types_string() const { return image_types_str_;}

    QFont get_fixed_width_code_font();

public Q_SLOTS:

    void set_start_frame( int t);
    void set_end_frame( int t);
    void set_frame( int t);

private:

    void init_ui_style();
    void save_window_state();
    void restore_window_state();

    main_window_t *window_;
    inspector_t *inspector_;
    anim_editor_t *anim_editor_;
    viewer_t *viewer_;

    node_t *active_, *context_;

    bool rendering_;
    bool interacting_;

    QString image_types_str_;

    // long calculations
    bool event_filter_installed_;
    bool cancelled_;
};

} // namespace
} // namespace

#endif
