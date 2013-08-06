// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/main_window.hpp>

#include<iostream>
#include<map>

#include<boost/foreach.hpp>

#include<QApplication>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QMessageBox>
#include<QVBoxLayout>
#include<QCloseEvent>
#include<QFileDialog>
#include<QDesktopServices>
#include<QDockWidget>
#include<QDesktopWidget>
#include<QUrl>
#include<QStatusBar>
#include<QSettings>
#include<QStringList>
#include<QFileInfo>
#include<QToolBar>

#include<ramen/version.hpp>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/memory/manager.hpp>

#include<ramen/nodes/graph_algorithm.hpp>
#include<ramen/nodes/node_factory.hpp>
#include<ramen/nodes/node_output_interface.hpp>
#include<ramen/nodes/image_node.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/render/render_sequence.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/inspector/inspector.hpp>
#include<ramen/ui/add_node_command.hpp>
#include<ramen/ui/edit_commands.hpp>
#include<ramen/ui/time_controls.hpp>
#include<ramen/ui/compview/composition_view.hpp>
#include<ramen/ui/viewer/viewer.hpp>
#include<ramen/ui/anim/anim_editor.hpp>
#include<ramen/ui/render_composition.hpp>
#include<ramen/ui/widgets/time_slider.hpp>
#include<ramen/ui/dialogs/render_composition_dialog.hpp>
#include<ramen/ui/dialogs/composition_settings_dialog.hpp>
#include<ramen/ui/dialogs/preferences_dialog.hpp>
#include<ramen/ui/dialogs/multiline_alert.hpp>

namespace ramen
{
namespace ui
{

const int main_window_t::max_recently_opened_files = 5;

const char *main_window_t::document_extension()		{ return ".rmn";}
const char *main_window_t::file_dialog_extension()	{ return "Ramen Composition (*.rmn)";}

main_window_t::main_window_t() : QMainWindow()
{
    menubar_ = menuBar();

    recently_opened_.assign( max_recently_opened_files, (QAction *) 0);

    time_controls_.reset( new time_controls_t());

    create_actions();
    create_menus();

    // docks
    setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    // Inspector
    inspector_dock_ = new QDockWidget( "Inspector", this);
    inspector_dock_->setObjectName( "inspector_dock");
    inspector_dock_->setAllowedAreas( Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea | Qt::BottomDockWidgetArea);
    inspector_dock_->setWidget( app().ui()->inspector().widget());
    add_dock_widget( Qt::RightDockWidgetArea, inspector_dock_);

    // anim editor dock
    anim_editor_dock_ = new QDockWidget( "Curve Editor", this);
    anim_editor_dock_->setObjectName( "anim_editor_dock");
    anim_editor_dock_->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea |
                                        Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    anim_editor_dock_->setWidget( app().ui()->anim_editor().widget());
    add_dock_widget( Qt::BottomDockWidgetArea, anim_editor_dock_);

    // Composition view
    {
        composition_dock_ = new QDockWidget( "Composition", this);
        composition_dock_->setObjectName( "composition_dock");
        composition_dock_->setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea |
                                            Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);


        QWidget *all_comp_view = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout();
        layout->setContentsMargins( 5, 5, 5, 5);

        comp_view_ = new composition_view_t();
        layout->addWidget( comp_view_);

        QFrame *separator = new QFrame();
        separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
        separator->setLineWidth( 1);
        layout->addWidget( separator);

        layout->addWidget( composition_view().create_toolbar());
        all_comp_view->setLayout( layout);

        composition_dock_->setWidget( all_comp_view);
        add_dock_widget( Qt::LeftDockWidgetArea, composition_dock_);
    }

    // image view
    setCentralWidget( app().ui()->viewer().widget() );

    // time toolbar
    addToolBar( Qt::BottomToolBarArea, create_time_toolbar());

    // create the status bar
    statusBar()->showMessage( RAMEN_NAME_FULL_VERSION_STR);

    QRect screen_size = qApp->desktop()->availableGeometry();
    move( screen_size.left(), screen_size.top());
    resize( screen_size.width(), screen_size.height() - 40);
    setWindowTitle( "Ramen");

    setWindowIcon( QIcon( ":small_app_icon.png"));
}

main_window_t::~main_window_t()
{
    // Do not remove, it's needed by auto_ptr
    // to handle correctly incomplete types.
}

QToolBar *main_window_t::create_time_toolbar()
{
    QToolBar *toolbar = new QToolBar( "Time Controls");
    toolbar->setObjectName( "time_controls");
    toolbar->setFloatable( false);
    toolbar->setMovable( false);

    time_slider_ = new time_slider_t();
    time_slider_->setSizePolicy( QSizePolicy::Expanding, time_slider_->sizePolicy().verticalPolicy());
    connect( time_slider_, SIGNAL( start_frame_changed( int)), app().ui(), SLOT( set_start_frame( int)));
    connect( time_slider_, SIGNAL( end_frame_changed( int)), app().ui(), SLOT( set_end_frame( int)));
    connect( time_slider_, SIGNAL( time_changed( int)), app().ui(), SLOT( set_frame( int)));

    toolbar->addWidget( time_slider_);
    toolbar->addSeparator();

    toolbar->addWidget( time_controls_->widget());
    return toolbar;
}

void main_window_t::add_dock_widget( Qt::DockWidgetArea area, QDockWidget *dock)
{
    addDockWidget( area, dock);
    view_->addAction( dock->toggleViewAction());
}

void main_window_t::closeEvent( QCloseEvent *event)
{
    quit();

    if( app().quitting())
        event->accept();
    else
        event->ignore();
}

void main_window_t::create_actions()
{
    new_ = new QAction( "New", this);
    new_->setShortcut( QString( "Ctrl+N"));
    new_->setShortcutContext( Qt::ApplicationShortcut);
    connect( new_, SIGNAL( triggered()), this, SLOT( new_document()));

    open_ = new QAction( "Open...", this);
    open_->setShortcut( QString( "Ctrl+O"));
    open_->setShortcutContext( Qt::ApplicationShortcut);
    connect( open_, SIGNAL( triggered()), this, SLOT( open_document()));

    for( int i = 0; i < max_recently_opened_files; ++i)
    {
        recently_opened_[i] = new QAction( this);
        recently_opened_[i]->setVisible( false);
        connect( recently_opened_[i], SIGNAL( triggered()), this, SLOT( open_recent_document()));
    }

    save_ = new QAction( "Save", this);
    save_->setShortcut( QString( "Ctrl+S"));
    save_->setShortcutContext( Qt::ApplicationShortcut);
    connect( save_, SIGNAL( triggered()), this, SLOT( save_document()));

    save_as_ = new QAction( "Save As...", this);
    connect( save_as_, SIGNAL( triggered()), this, SLOT( save_document_as()));

    quit_ = new QAction( "Quit", this);
    quit_->setShortcut( QString( "Ctrl+Q"));
    quit_->setShortcutContext( Qt::ApplicationShortcut);
    connect( quit_, SIGNAL( triggered()), this, SLOT( quit()));

    import_comp_ = new QAction( "Import Composition...", this);
    connect( import_comp_, SIGNAL( triggered()), this, SLOT( import_composition()));

    export_sel_ = new QAction( "Export Selection...", this);
    export_sel_->setEnabled( false);
    connect( export_sel_, SIGNAL( triggered()), this, SLOT( export_selection()));

    undo_ = new QAction( "Undo", this);
    undo_->setShortcut( QString( "Ctrl+Z"));
    undo_->setShortcutContext( Qt::ApplicationShortcut);
    connect( undo_, SIGNAL( triggered()), this, SLOT( undo()));

    redo_ = new QAction( "Redo", this);
    redo_->setShortcut( QString( "Ctrl+Shift+Z"));
    redo_->setShortcutContext( Qt::ApplicationShortcut);
    connect( redo_, SIGNAL( triggered()), this, SLOT( redo()));

    ignore_ = new QAction( "Ignore", this);
    ignore_->setShortcut( QString( "Ctrl+I"));
    ignore_->setShortcutContext( Qt::ApplicationShortcut);
    connect( ignore_, SIGNAL( triggered()), this, SLOT( ignore_nodes()));

    delete_ = new QAction( "Delete", this);
    connect( delete_, SIGNAL( triggered()), this, SLOT( delete_nodes()));

    duplicate_ = new QAction( "Duplicate", this);
    duplicate_->setShortcut( QString( "Ctrl+D"));
    duplicate_->setShortcutContext( Qt::ApplicationShortcut);
    connect( duplicate_, SIGNAL( triggered()), this, SLOT( duplicate_nodes()));

    extract_ = new QAction( "Extract", this);
    connect( extract_, SIGNAL( triggered()), this, SLOT( extract_nodes()));

    group_ = new QAction( "Group", this);
    ungroup_ = new QAction( "Ungroup", this);

    clear_cache_ = new QAction( "Clear Image Cache", this);
    connect( clear_cache_, SIGNAL( triggered()), this, SLOT( clear_cache()));

    preferences_ = new QAction( "Preferences...", this);
    connect( preferences_, SIGNAL( triggered()), this, SLOT( show_preferences_dialog()));

    comp_settings_ = new QAction( "Composition Settings...", this);
    comp_settings_->setShortcut( QString( "Ctrl+K"));
    comp_settings_->setShortcutContext( Qt::ApplicationShortcut);
    connect( comp_settings_, SIGNAL( triggered()), this, SLOT( show_composition_settings_dialog()));

    comp_flipbook_ = new QAction( "Render Flipbook...", this);
    connect( comp_flipbook_, SIGNAL( triggered()), this, SLOT( render_flipbook()));

    comp_render_ = new QAction( "Render Composition...", this);
    comp_render_->setShortcut( QString( "Ctrl+R"));
    comp_render_->setShortcutContext( Qt::ApplicationShortcut);
    connect( comp_render_, SIGNAL( triggered()), this, SLOT( render_composition()));

    about_ = new QAction( "About Ramen...", this);
    connect( about_, SIGNAL( triggered()), this, SLOT( show_about_box()));

    project_web_ = new QAction( tr( "Project Website..."), this);
    connect( project_web_, SIGNAL( triggered()), this, SLOT( go_to_project_website()));

    // non-menu actions
    next_frame_ = new QAction( this);
    next_frame_->setShortcut( Qt::Key_Right);
    next_frame_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
    connect( next_frame_, SIGNAL( triggered()), &(time_controls()), SLOT( next_frame()));
    addAction( next_frame_);

    prev_frame_ = new QAction( this);
    prev_frame_->setShortcut( Qt::Key_Left);
    prev_frame_->setShortcutContext( Qt::WidgetWithChildrenShortcut);
    connect( prev_frame_, SIGNAL( triggered()), &(time_controls()), SLOT( prev_frame()));
    addAction( prev_frame_);
}

void main_window_t::create_menus()
{
    file_ = menubar_->addMenu( "File");
    file_->addAction( new_);
    file_->addAction( open_);

    open_recent_ = file_->addMenu( "Open Recent");
    for( int i = 0; i < max_recently_opened_files; ++i)
        open_recent_->addAction( recently_opened_[i]);

    init_recent_files_menu();

    file_->addAction( save_);
    file_->addAction( save_as_);
    file_->addSeparator();

    import_ = file_->addMenu( "Import");
    export_ = file_->addMenu( "Export");
    create_import_export_menus();

    file_->addSeparator();
    file_->addAction( quit_);

    edit_ = menubar_->addMenu( "Edit");
    edit_->addAction( undo_);
    edit_->addAction( redo_);
    edit_->addSeparator();
    edit_->addAction( ignore_);
    edit_->addAction( delete_);
    edit_->addAction( duplicate_);
    edit_->addAction( extract_);
    edit_->addSeparator();
    //edit_->addAction( group_);
    //edit_->addAction( ungroup_);
    //edit_->addSeparator();
    edit_->addAction( clear_cache_);
    edit_->addSeparator();
    edit_->addAction( preferences_);

    composition_ = menubar_->addMenu( "Composition");
    composition_->addAction( comp_settings_);
    composition_->addSeparator();
    composition_->addAction( comp_flipbook_);
    composition_->addAction( comp_render_);

    create_node_actions();

    for( int i = 0; i < node_menus_.size(); ++i)
    {
        node_menus_[i]->menu()->setTearOffEnabled( true);
        menubar_->addMenu( node_menus_[i]->menu());
    }

    view_ = menubar_->addMenu( "Window");

    help_ = menubar_->addMenu( "Help");
    help_->addAction( about_);
    help_->addAction( project_web_);
}

void main_window_t::create_import_export_menus()
{
    //import_->addAction( import_comp_);
    //export_->addAction( export_sel_);
}

node_menu_t *main_window_t::find_node_menu( const std::string& s)
{
    for( int i = 0; i < node_menus_.size(); ++i)
    {
        if( node_menus_[i]->name() == s)
            return node_menus_[i];
    }

    node_menu_t *m = new node_menu_t( s);
    node_menus_.push_back( m);
    return m;
}

void main_window_t::create_node_actions()
{
    // Add some default menus & submenus
    node_menu_t *m = new node_menu_t( "Image");
    node_menus_.push_back( m);
    m->add_submenu( "Input");
    m->add_submenu( "Channel");
    m->add_submenu( "Color");
    m->add_submenu( "Key");
    m->add_submenu( "Matte");
    m->add_submenu( "Filter");
    m->add_submenu( "Noise");
    m->add_submenu( "Distort");
    //m->add_submenu( "Inpaint");
    m->add_submenu( "Transform");
    m->add_submenu( "Track");
    m->add_submenu( "Layer");
    //m->add_submenu( "Render");
    m->add_submenu( "Lighting");
    m->add_submenu( "Tonemap");
    m->add_submenu( "Time");
    m->add_submenu( "Util");
    //m->add_submenu( "Video");
    m->add_submenu( "Output");

    // sort the list of registered nodes
    node_factory_t::instance().sort_by_menu_item();

    // add our builtin nodes first
    BOOST_FOREACH( const node_metaclass_t& mclass, node_factory_t::instance().registered_nodes())
    {
        if( mclass.ui_visible && node_factory_t::instance().is_latest_version( mclass.id))
        {
            node_menu_t *menu = find_node_menu( mclass.menu);
            QAction *act = new QAction( QString( mclass.menu_item.c_str()), this);
            connect( act, SIGNAL( triggered()), this, SLOT( create_node()));
            create_node_actions_[act] = mclass.id;
            menu->add_action( mclass.submenu, act);
        }
    }
}

const std::vector<node_menu_t*>& main_window_t::node_menus() const
{
    RAMEN_ASSERT( !node_menus_.empty());
    return node_menus_;
}

bool main_window_t::can_close_document()
{
    if( app().document().dirty())
    {
        int r = QMessageBox::warning( this, "Ramen", "The composition has been modified.\n"
                                      "Do you want to save your changes?", QMessageBox::Yes | QMessageBox::Default,
                                      QMessageBox::No, QMessageBox::Cancel | QMessageBox::Escape);

        if (r == QMessageBox::Yes)
        {
            save_document();
            return app().document().dirty();
        }
        else
            if( r == QMessageBox::Cancel)
                return false;
    }

    return true;
}

void main_window_t::new_document()
{
    if( can_close_document())
        app().ui()->create_new_document();
}

void main_window_t::open_document()
{
    if( can_close_document())
    {
        QString fname = QFileDialog::getOpenFileName( 0, "Open Composition", QString::null, file_dialog_extension(),
                                                      0, QFileDialog::DontUseNativeDialog);

        if( !( fname.isEmpty()))
        {
            boost::filesystem::path p( fname.toStdString());
            app().ui()->open_document( p);
        }
    }
}

void main_window_t::open_recent_document()
{
    if( can_close_document())
    {
        QAction *action = qobject_cast<QAction *>(sender());

        if( action)
        {
            QString fname = action->data().toString();
            boost::filesystem::path p( fname.toStdString());
            app().ui()->open_document( p);
        }
    }
}

void main_window_t::save_document()
{
    if( app().document().has_file())
    {
        app().ui()->save_document();
        app().ui()->update();
    }
    else
        save_document_as();
}

void main_window_t::save_document_as()
{
    QString fname = QFileDialog::getSaveFileName( 0, "Save Composition As", QString::null, file_dialog_extension(),
                                                  0, QFileDialog::DontUseNativeDialog);

    if( !(fname.isEmpty()))
    {
        boost::filesystem::path p( fname.toStdString());

        if( p.extension() == std::string())
            p.replace_extension( document_extension());

        boost::filesystem::path old_file = app().document().file();
        app().document().set_file( p);

        if( !app().ui()->save_document())
        {
            // save was not successful, restore the relative paths
            // to the state before trying to save.
            if( !old_file.empty())
                app().document().set_file( old_file);
        }
        else
        {
            update_recent_files_menu( app().document().file());
            app().document().undo_stack().clear_all();
        }

        app().ui()->update();
    }
}

void main_window_t::import_composition()
{
    QString fname = QFileDialog::getOpenFileName( 0, "Open Composition", QString::null, file_dialog_extension(),
                                                  0, QFileDialog::DontUseNativeDialog);

    if( !( fname.isEmpty()))
    {
        try
        {
            /*
            boost::filesystem::path p( fname.toStdString());
            std::auto_ptr<serialization::yaml_iarchive_t> in( ramen::import_composition( p));
            app().document().set_dirty( true);
            app().document().undo_stack().clear_all();

            // report errors to the user
            std::string err = in->errors();

            if( !err.empty())
                multiline_alert_t::instance().show_alert( "Errors during file open", err);
            */
        }
        catch( std::exception& e)
        {
            QMessageBox::warning( this, "Error opening document", "Old version or corrupted composition");
        }

        app().ui()->update();
    }
}

void main_window_t::export_selection()
{
    /*
    QString fname = QFileDialog::getSaveFileName( 0, "Save Composition As", QString::null, file_dialog_extension(),
                                                  0, QFileDialog::DontUseNativeDialog);

    if( !(fname.isEmpty()))
    {
        boost::filesystem::path p( fname.toStdString());

        if( p.extension() == std::string())
            p.replace_extension( document_extension());

        export_selected_nodes( p);
    }
    */
}

void main_window_t::quit()
{
    if( app().document().dirty())
    {
        int r = QMessageBox::warning( this,
                                      "Ramen",
                                      "The composition has been modified.\n"
                                      "Do you want to save your changes before quitting?",
                                      QMessageBox::Yes | QMessageBox::Default,
                                      QMessageBox::No,
                                      QMessageBox::Cancel | QMessageBox::Escape);

        switch( r)
        {
            case QMessageBox::Yes:
            {
                save_document();

                // if the document is still dirty, it means
                // save was cancelled, so we return without quitting
                if( app().document().dirty())
                    return;
            }
            break;

            case QMessageBox::No:
                app().ui()->quit();
            break;

            case QMessageBox::Cancel:
            return;
        }
    }

    app().ui()->quit();
}

void main_window_t::undo()
{
    app().document().undo_stack().undo();
    app().ui()->update();
}

void main_window_t::redo()
{
    app().document().undo_stack().redo();
    app().ui()->update();
}

void main_window_t::ignore_nodes()
{
    std::auto_ptr<undo::ignore_nodes_command_t> c( new undo::ignore_nodes_command_t());

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
        if( n.selected())
            c->add_node( &n);
    }

    c->redo();
    app().document().undo_stack().push_back( c);
    app().ui()->update();
}

void main_window_t::delete_nodes()
{
    bool autoconnect = true;

    if( !app().document().composition().any_selected())
        return;

    std::auto_ptr<undo::delete_command_t> c( new undo::delete_command_t());

    BOOST_FOREACH( edge_t& e, app().document().composition().edges())
    {
        if( e.src->selected() && !(e.dst->selected()))
            c->add_dependent_node( e.dst);
    }

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
        if( n.selected())
            c->add_node( &n);
    }

    BOOST_FOREACH( edge_t& e, app().document().composition().edges())
    {
        if( e.src->selected() || e.dst->selected())
            c->add_edge_to_remove( e);
    }

    if( autoconnect)
    {
        std::vector<edge_t> edges_to_add;

        BOOST_FOREACH( node_t& n, app().document().composition().nodes())
        {
            if( n.selected())
            {
                if( n.num_outputs() == 0)
                    continue;

                node_t *src = 0;

                // find first input
                for( int i = 0; i < n.num_inputs(); ++i)
                {
                    if( n.input( i) && !n.input( i)->selected())
                    {
                        src = n.input( i);
                        break;
                    }
                }

                if( src)
                {
                    breadth_first_out_edges_apply( n,
                                                   boost::bind( &undo::delete_command_t::add_candidate_edge,
                                                   _1,
                                                   src,
                                                   boost::ref( edges_to_add)));
                }
            }
        }

        std::stable_sort( edges_to_add.begin(), edges_to_add.end(),
                          &undo::delete_command_t::edge_less);
        std::unique( edges_to_add.begin(), edges_to_add.end(),
                     &undo::delete_command_t::edge_compare);

        for( int i = 0; i < edges_to_add.size(); ++i)
            c->add_edge_to_add( edges_to_add[i]);
    }

    c->redo();
    app().document().undo_stack().push_back( c);
    app().ui()->update();
}

void main_window_t::duplicate_nodes()
{
    std::map<node_t*, node_t*> relation;
    std::auto_ptr<undo::duplicate_command_t> c( new undo::duplicate_command_t());

    BOOST_FOREACH( node_t& n, app().document().composition().nodes())
    {
        if( n.selected())
        {
            std::auto_ptr<node_t> nclone( new_clone( n));
            nclone->offset_location( Imath::V2f( 20, 20));
            relation[ &n] = nclone.get();
            c->add_node( nclone);
        }
    }

    BOOST_FOREACH( edge_t& e, app().document().composition().edges())
    {
        if( e.src->selected() && e.dst->selected())
            c->add_edge( edge_t( relation[e.src], relation[e.dst], e.port));
    }

    app().document().composition().deselect_all();
    c->redo();
    app().document().undo_stack().push_back( c);
    app().ui()->update();
}

void main_window_t::extract_nodes()
{
    bool autoconnect = true;

    if( !app().document().composition().any_selected())
        return;

    std::auto_ptr<undo::extract_command_t> c( new undo::extract_command_t());

    BOOST_FOREACH( edge_t& e, app().document().composition().edges())
    {
        if( e.src->selected() && !(e.dst->selected()))
            c->add_dependent_node( e.dst);
    }

    BOOST_FOREACH( edge_t& e, app().document().composition().edges())
    {
        if( e.src->selected() || e.dst->selected())
            c->add_edge_to_remove( e);
    }

    if( autoconnect)
    {
        std::vector<edge_t> edges_to_add;

        BOOST_FOREACH( node_t& n, app().document().composition().nodes())
        {
            if( n.selected())
            {
                if( n.num_outputs() == 0)
                    continue;

                node_t *src = 0;

                // find first input
                for( int i = 0; i < n.num_inputs(); ++i)
                {
                    if( n.input( i) && !n.input( i)->selected())
                    {
                        src = n.input( i);
                        break;
                    }
                }

                if( src)
                    breadth_first_out_edges_apply( n,
                                                   boost::bind( &undo::delete_command_t::add_candidate_edge,
                                                                _1,
                                                                src,
                                                                boost::ref( edges_to_add)));
            }
        }

        std::stable_sort( edges_to_add.begin(), edges_to_add.end(),
                          &undo::delete_command_t::edge_less);

        std::unique( edges_to_add.begin(), edges_to_add.end(),
                     &undo::delete_command_t::edge_compare);

        for( int i = 0; i < edges_to_add.size(); ++i)
            c->add_edge_to_add( edges_to_add[i]);
    }

    c->redo();
    app().document().undo_stack().push_back( c);
    app().ui()->update();
}

void main_window_t::clear_cache()
{
    app().memory_manager().clear_caches();
}

void main_window_t::show_composition_settings_dialog()
{
    composition_settings_dialog_t::instance().exec_dialog();
}

void main_window_t::render_composition()
{
    bool any_output = ( render::count_output_nodes( app().document().composition()) != 0);

    if( !any_output)
    {
        QMessageBox::warning( this, "Ramen", "No output nodes in composition");
        return;
    }

    bool any_output_selected = ( render::count_output_nodes( app().document().composition(), true) != 0);

    render_composition_dialog_t::instance().set_any_output_selected( any_output_selected);

    int result = render_composition_dialog_t::instance().exec();
    if( result == QDialog::Accepted)
    {
        int start = render_composition_dialog_t::instance().start_frame();
        int end = render_composition_dialog_t::instance().end_frame();

        if( end < start)
            return;

        ui::render_composition( app().document().composition(),
                                start, end, render_composition_dialog_t::instance().proxy_level(),
                                render_composition_dialog_t::instance().resolution(),
                                render_composition_dialog_t::instance().mblur_extra_samples(),
                                render_composition_dialog_t::instance().mblur_shutter_factor(),
                                render_composition_dialog_t::instance().selected_only());
    }
}

void main_window_t::render_flipbook()
{
    /*
    int result = render_flipbook_dialog_t::instance().exec();

    if( result == QDialog::Accepted)
    {
        if( image_node_t *n = dynamic_cast<image_node_t*>( app().document().composition().selected_node()))
        {
            int frame_rate = app().document().composition().frame_rate();
            std::string display_device = render_flipbook_dialog_t::instance().display_device();
            std::string display_transform = render_flipbook_dialog_t::instance().display_transform();
            int start	= render_flipbook_dialog_t::instance().start_frame();
            int end	= render_flipbook_dialog_t::instance().end_frame();
            int subsample =  render_flipbook_dialog_t::instance().resolution();
            int proxy_level = render_flipbook_dialog_t::instance().proxy_level();
            int mb_extra_samples = render_flipbook_dialog_t::instance().mblur_extra_samples();
            float mb_shutter_factor = render_flipbook_dialog_t::instance().mblur_shutter_factor();

            flipbook::flipbook_t *flip = flipbook::factory_t::instance().create( render_flipbook_dialog_t::instance().flipbook(),
                                                                                   frame_rate, display_device, display_transform);

            if( flip)
            {
                if( flipbook::render_flipbook( flip, n, start, end, proxy_level, subsample, mb_extra_samples, mb_shutter_factor))
                    flip->play();
            }
        }
    }
    */
}

void main_window_t::show_preferences_dialog()
{
    preferences_dialog_t::instance().exec_dialog();
}

void main_window_t::create_node()
{
    QAction *action = dynamic_cast<QAction*>( sender());

    std::string id( create_node_actions_[action]);
    std::auto_ptr<node_t> p( node_factory_t::instance().create_by_id( id, true));

    if( !p.get())
    {
        //app().ui()->error( std::string( "Couldn't create node ") + id);
        return;
    }

    try
    {
        p->set_composition( &( app().document().composition()));
        p->create_params();
        p->create_manipulators();
    }
    catch( std::exception& e)
    {
        app().ui()->error( std::string( "Couldn't create node ") + id
                           + std::string( " ") + e.what());
        return;
    }

    // test to see if we can autoconnect
    node_t *src = app().document().composition().selected_node();

    if( src && src->has_output_plug() && p->num_inputs() != 0)
    {
        if( !app().document().composition().can_connect( src, p.get(), 0))
            src = 0;
    }
    else
        src = 0;

    if( src)
        composition_view().place_node_near_node( p.get(), src);
    else
        composition_view().place_node( p.get());

    node_t *n = p.get(); // save for later use
    std::auto_ptr<undo::command_t> c( new undo::add_node_command_t( p, src));
    app().document().composition().deselect_all();
    n->select( true);
    c->redo();
    app().document().undo_stack().push_back( c);
    app().ui()->update();
}

void main_window_t::show_about_box()
{
	QMessageBox msgBox;
	msgBox.setTextFormat(Qt::RichText);
	msgBox.setWindowTitle( "About Ramen");
	QString text = QString( "<html><p style=\" font-size:16pt;"
					        "font-weight:bold;\">%1</p></html>").arg(RAMEN_NAME_FULL_VERSION_STR);
	msgBox.setText( text);
	msgBox.setInformativeText( "For full license details please read the LICENSE.txt file");
	// Add spacer to force width of QMessageBox
	QSpacerItem* horizontalSpacer = new QSpacerItem( 400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout* layout = (QGridLayout*)msgBox.layout();
	layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
	msgBox.exec();
}

void main_window_t::go_to_project_website()
{
	QDesktopServices::openUrl(QUrl("http://ramencomp.blogspot.com/",
									QUrl::TolerantMode));
}

void main_window_t::update()
{
    if( app().document().dirty())
        setWindowTitle( "Ramen *");
    else
        setWindowTitle( "Ramen");

    update_menus();
    time_slider_->update( app().document().composition().start_frame(),
                          app().document().composition().frame(),
                          app().document().composition().end_frame());

    composition_view().update();
    time_controls_->update();
}

void main_window_t::update_recent_files_menu( const boost::filesystem::path& p)
{
    QString fileName( filesystem::file_cstring( p));

    QSettings settings( "com.ramen.qt", "Ramen Recent Files");
    QStringList files = settings.value( "recent_file_list").toStringList();
    files.removeAll( fileName);
    files.prepend( fileName);

    while( files.size() > max_recently_opened_files)
        files.removeLast();

    settings.setValue("recent_file_list", files);

    int num_recent_files = std::min( files.size(), (int) max_recently_opened_files);

    for( int i = 0; i < num_recent_files; ++i)
    {
        QString stripped = QFileInfo( files[i]).fileName();
        QString text = tr("&%1 %2").arg(i + 1).arg( stripped);
        recently_opened_[i]->setText( text);
        recently_opened_[i]->setData( files[i]);
        recently_opened_[i]->setVisible( true);
    }

    for( int j = num_recent_files; j < max_recently_opened_files; ++j)
        recently_opened_[j]->setVisible( false);
}

void main_window_t::init_recent_files_menu()
{
    QSettings settings( "com.ramen.qt", "Ramen Recent Files");
    QStringList files = settings.value( "recent_file_list").toStringList();

    int num_recent_files = std::min( files.size(), (int) max_recently_opened_files);

    for( int i = 0; i < num_recent_files; ++i)
    {
        QString stripped = QFileInfo( files[i]).fileName();
        QString text = tr("&%1 %2").arg(i + 1).arg( stripped);
        recently_opened_[i]->setText( text);
        recently_opened_[i]->setData( files[i]);
        recently_opened_[i]->setVisible( true);
    }

    for( int j = num_recent_files; j < max_recently_opened_files; ++j)
        recently_opened_[j]->setVisible( false);
}

void main_window_t::update_menus()
{
    bool any_selected = app().document().composition().any_selected();
    node_t *n = app().document().composition().selected_node();

    save_->setEnabled( app().document().dirty());
    export_sel_->setEnabled( any_selected);

    if( app().document().undo_stack().undo_empty())
    {
        undo_->setText( "Undo");
        undo_->setEnabled( false);
    }
    else
    {
        undo_->setText( QString( "Undo ") + app().document().undo_stack().last_undo_command().name().c_str());
        undo_->setEnabled( true);
    }

    if( app().document().undo_stack().redo_empty())
    {
        redo_->setText( "Redo");
        redo_->setEnabled( false);
    }
    else
    {
        redo_->setText( QString( "Redo ") + app().document().undo_stack().last_redo_command().name().c_str());
        redo_->setEnabled( true);
    }

    ignore_->setEnabled( any_selected);
    delete_->setEnabled( any_selected);
    duplicate_->setEnabled( any_selected);
    extract_->setEnabled( any_selected);

    group_->setEnabled( false);
    ungroup_->setEnabled( false);

    if( n && dynamic_cast<image_node_t*>( n))
        comp_flipbook_->setEnabled( true);
    else
        comp_flipbook_->setEnabled( false);
}

} // ui
} // ramen
