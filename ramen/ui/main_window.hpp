// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_MAIN_WINDOW_HPP
#define	RAMEN_UI_MAIN_WINDOW_HPP

#include<ramen/ui/main_window_fwd.hpp>

#include<vector>
#include<map>
#include<string>
#include<memory>

#include<QMainWindow>
#include<QDockWidget>

#include<boost/filesystem/fstream.hpp>

#include<ramen/core/memory.hpp>

#include<ramen/serialization/archive_fwd.hpp>

#include<ramen/ui/node_menu.hpp>
#include<ramen/ui/compview/composition_view_fwd.hpp>
#include<ramen/ui/time_controls_fwd.hpp>
#include<ramen/ui/widgets/time_slider_fwd.hpp>

class QAction;
class QMenu;
class QMenuBar;
class QDockWidget;
class QToolBar;
class QrTimeSlider;

namespace ramen
{
namespace ui
{

/**
\ingroup ui
\brief main window class.
*/
class main_window_t : public QMainWindow
{
    Q_OBJECT

public:

    main_window_t();
    ~main_window_t();

    void add_dock_widget( Qt::DockWidgetArea area, QDockWidget *dock);

    const composition_view_t& composition_view() const  { return *comp_view_;}
    composition_view_t& composition_view()              { return *comp_view_;}

    const time_slider_t& time_slider() const    { return *time_slider_;}
    time_slider_t& time_slider()                { return *time_slider_;}

    const time_controls_t& time_controls() const    { return *time_controls_;}
    time_controls_t& time_controls()                { return *time_controls_;}

    const std::vector<node_menu_t*>& node_menus() const;

    void update();

    bool can_close_document();

    void update_recent_files_menu( const boost::filesystem::path& p);

protected:

    void closeEvent( QCloseEvent *event);

public Q_SLOTS:

    void new_document();
    void open_document();
    void open_recent_document();

    void save_document();
    void save_document_as();

    void import_composition();

    void export_selection();

    void quit();

    void undo();
    void redo();

    void ignore_nodes();
    void delete_nodes();
    void duplicate_nodes();
    void extract_nodes();

    void clear_cache();

    void show_preferences_dialog();

    void show_composition_settings_dialog();

    void render_flipbook();
    void render_composition();

    void show_about_box();
    void go_to_project_website();

    void create_node();

private:

    QToolBar *create_time_toolbar();

    void create_actions();
    void create_menus();
    void create_import_export_menus();

    void create_node_actions();

    void init_recent_files_menu();
    void update_menus();

    node_menu_t *find_node_menu( const std::string& s);

    static const char *document_extension();
    static const char *file_dialog_extension();

    time_slider_t *time_slider_;
    composition_view_t *comp_view_;
    core::auto_ptr_t<time_controls_t> time_controls_;

    QDockWidget *inspector_dock_;
    QDockWidget *composition_dock_;
    QDockWidget *viewer_dock_;
    QDockWidget *anim_editor_dock_;

    QMenuBar *menubar_;

    QMenu *file_, *edit_, *composition_, *view_, *help_;
    QMenu *open_recent_;
    QMenu *import_, *export_;

    std::vector<QAction *> recently_opened_;

    std::vector<QMenu*> create_submenus_;

    // file
    QAction *new_, *open_, *save_, *save_as_;

    QAction *import_comp_;

    QAction *export_sel_;

    QAction *quit_;

    // edit
    QAction *undo_, *redo_, *ignore_, *delete_;
    QAction *duplicate_, *group_, *ungroup_, *extract_;
    QAction *clear_cache_, *preferences_;

    // comp
    QAction *comp_settings_, *comp_flipbook_, *comp_render_;

    // more
    QAction *about_, *project_web_;

    // non - menu actions
    QAction *next_frame_, *prev_frame_;

    std::vector<node_menu_t*> node_menus_;
    std::map<QAction*,std::string> create_node_actions_;

    const static int max_recently_opened_files;
};

} // namespace
} // namespace

#endif

