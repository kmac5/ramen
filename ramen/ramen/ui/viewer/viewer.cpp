// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ui/viewer/viewer.hpp>

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QFrame>
#include<QPushButton>
#include<QComboBox>
#include<QToolButton>
#include<QLabel>

#include<ramen/app/application.hpp>

#include<ramen/nodes/node.hpp>

#include<ramen/ocio/manager.hpp>

#include<ramen/ui/viewer/viewer_context.hpp>
#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/widgets/double_spinbox.hpp>
#include<ramen/ui/widgets/container_widget.hpp>

namespace ramen
{
namespace ui
{

viewer_t::viewer_t() : QObject()
{
    // TODO: don't harcode this. Get it from some widgets
    toolbar_height_ = 30;

    current_node_toolbar_ = node_toolbars_.end();

    // create first the ocio widgets to init the list of devices & displays
    ocio_device_combo_ = new QComboBox();
    get_display_devices();

    ocio_transform_combo_ = new QComboBox();
    get_display_transforms();

    // normal widgets
    window_ = new QWidget();
    window_->setWindowTitle( "Viewer");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins( 5, 5, 5, 5);

    QWidget *status_bar = new QWidget();
    QHBoxLayout *status_layout = new QHBoxLayout();
    status_layout->setContentsMargins( 0, 0, 0, 0);
    status_ = new QLabel();
    status_->setText( "Status");
    status_layout->addWidget( status_);

    QSize label_size = status_->sizeHint();
    status_bar->setMinimumSize( 0, label_size.height());
    status_bar->setMaximumSize( QWIDGETSIZE_MAX, label_size.height());

    status_bar->setLayout( status_layout);
    layout->addWidget( status_bar);

    view_ = new viewer::viewer_context_t();
    layout->addWidget( view_);

    active_toolbar_ = new container_widget_t();
    active_toolbar_->setMinimumSize( 0, toolbar_height());
    active_toolbar_->setMaximumSize( QWIDGETSIZE_MAX, toolbar_height());
    active_toolbar_->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
    layout->addWidget( active_toolbar_);

    QFrame *separator = new QFrame();
    separator->setFrameStyle( QFrame::HLine | QFrame::Raised);
    separator->setLineWidth( 1);
    layout->addWidget( separator);

    QWidget *viewer_controls = new QWidget();
    viewer_controls->setMinimumSize( 0, toolbar_height());
    viewer_controls->setMaximumSize( QWIDGETSIZE_MAX, toolbar_height());
    viewer_controls->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *horizontalLayout = new QHBoxLayout( viewer_controls);
    horizontalLayout->setContentsMargins( 0, 0, 0, 0);

	result_combo_ = new QComboBox();
	result_combo_->insertItems(0, QStringList() << "Active" << "Context");
	QSize s = result_combo_->sizeHint();

	update_btn_ = new QToolButton();
	update_btn_->setFocusPolicy( Qt::NoFocus);
	update_btn_->setCheckable(true);
	update_btn_->setChecked(true);
	update_btn_->setText( "U");
	update_btn_->setToolTip( "Auto-update");
	update_btn_->setMaximumWidth( s.height());
	update_btn_->setMaximumHeight( s.height());
	connect( update_btn_, SIGNAL( toggled( bool)), this, SLOT( autoupdate_toggle( bool)));
	horizontalLayout->addWidget(update_btn_);

	result_combo_->setFocusPolicy( Qt::NoFocus);
	result_combo_->setToolTip( "Show result / context node");
	connect( result_combo_, SIGNAL( activated( int)), this, SLOT( change_active_context_view( int)));
	horizontalLayout->addWidget(result_combo_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    horizontalLayout->addWidget( separator);

	ocio_device_combo_->setFocusPolicy( Qt::NoFocus);
	ocio_device_combo_->setToolTip( "Display Device");
	connect( ocio_device_combo_, SIGNAL( activated( int)), this, SLOT( change_display_device( int)));
	horizontalLayout->addWidget( ocio_device_combo_);

	ocio_transform_combo_->setFocusPolicy( Qt::NoFocus);
	ocio_transform_combo_->setToolTip( "Display Transform");
	connect( ocio_transform_combo_, SIGNAL( activated( int)), this, SLOT( change_display_transform( int)));
	horizontalLayout->addWidget( ocio_transform_combo_);

    exposure_input_ = new ui::double_spinbox_t();
	s = exposure_input_->sizeHint();
	exposure_input_->setMinimumSize( s);
	exposure_input_->setMaximumSize( s);
	exposure_input_->setMinimum(-30.0);
	exposure_input_->setMaximum( 30.0);
	exposure_input_->setSingleStep( 0.1);
	exposure_input_->setDecimals( 3);
	exposure_input_->setToolTip( "Viewer Exposure");
	connect( exposure_input_, SIGNAL( valueChanged( double)), this, SLOT( change_exposure( double)));
	connect( exposure_input_, SIGNAL( spinBoxDragged( double)), this, SLOT( change_exposure( double)));
	horizontalLayout->addWidget( exposure_input_);

    gamma_input_ = new ui::double_spinbox_t();
	gamma_input_->setMinimumSize( s);
	gamma_input_->setMaximumSize( s);
	gamma_input_->setMinimum( 0);
	gamma_input_->setMaximum( 4);
	gamma_input_->setValue( 1);
	gamma_input_->setSingleStep( 0.1);
	gamma_input_->setDecimals( 3);
	gamma_input_->setToolTip( "Viewer Gamma");
	connect( gamma_input_, SIGNAL( valueChanged( double)), this, SLOT( change_gamma( double)));
	connect( gamma_input_, SIGNAL( spinBoxDragged( double)), this, SLOT( change_gamma( double)));
	horizontalLayout->addWidget( gamma_input_);

    separator = new QFrame();
    separator->setFrameStyle( QFrame::VLine | QFrame::Raised);
    separator->setLineWidth( 1);
    horizontalLayout->addWidget( separator);

    viewer_toolbar_ = new container_widget_t();
    viewer_toolbar_->setMinimumSize( 0, toolbar_height());
    viewer_toolbar_->setMaximumSize( QWIDGETSIZE_MAX, toolbar_height());
    viewer_toolbar_->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed);
    horizontalLayout->addWidget( viewer_toolbar_);

    viewer_controls->setLayout( horizontalLayout);
    layout->addWidget( viewer_controls);
    window_->setLayout( layout);
}

viewer_t::~viewer_t() {}

int viewer_t::width() const	{ return view_->width();}
int viewer_t::height() const	{ return view_->height();}

int viewer_t::toolbar_height() const { return toolbar_height_;}

void viewer_t::get_display_devices()
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

    int index = 0;
    QStringList slist;

    std::string default_device_name = config->getDefaultDisplay();
    int num_device_names = config->getNumDisplays();

    for(int i = 0; i < num_device_names; i++)
    {
        std::string devname = config->getDisplay( i);
        display_devices_.push_back( devname);

        slist << devname.c_str();

        if( default_device_name == devname)
            index = i;
    }

    ocio_device_combo_->addItems( slist);
    ocio_device_combo_->setCurrentIndex( index);
    display_device_ = default_device_name;
}

void viewer_t::get_display_transforms()
{
    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

    std::string device_name = display_devices_[ ocio_device_combo_->currentIndex()];
    std::string default_transform = config->getDefaultView( device_name.c_str());

    int index = 0;
    QStringList slist;
    int num_transform_names = config->getNumViews( device_name.c_str());

    for( int i = 0; i < num_transform_names; ++i)
    {
        std::string trname = config->getView( device_name.c_str(), i);
        display_transforms_.push_back( trname);
        slist << trname.c_str();

        if( default_transform == trname)
            index = i;
    }

    ocio_transform_combo_->addItems( slist);
    ocio_transform_combo_->setCurrentIndex( index);
    display_transform_ = default_transform;
}

const std::string& viewer_t::display_device() const      { return display_device_;}
const std::string& viewer_t::display_transform() const   { return display_transform_;}

float viewer_t::exposure() const	{ return exposure_input_->value();}
float viewer_t::gamma() const	{ return gamma_input_->value();}

void viewer_t::update_display_transform() { view_->display_transform_changed();}

const viewer::viewer_strategy_t& viewer_t::current_viewer() const	{ return view_->strategy();}
viewer::viewer_strategy_t& viewer_t::current_viewer()				{ return view_->strategy();}

void viewer_t::set_viewer_toolbar( QWidget *w) { viewer_toolbar_->set_contents( w);}

void viewer_t::set_active_node( node_t *n)
{
    // handle the active toolbar
    if( current_node_toolbar_ != node_toolbars_.end())
    {
        active_toolbar_->clear_contents();
        current_node_toolbar_ = node_toolbars_.end();
    }

    if(n != 0)
    {
        std::map<node_t*,QWidget*>::const_iterator it = node_toolbars_.find( n);

        if( it == node_toolbars_.end())
        {
            std::auto_ptr<QWidget> toolbar = n->create_toolbar();
            node_toolbars_[ n] = toolbar.release();
        }

        current_node_toolbar_ = node_toolbars_.find( n);
        active_toolbar_->set_contents( current_node_toolbar_->second);
    }

    view_->set_active_node( n);
    view_->update();
}

void viewer_t::set_context_node( node_t *n) { view_->set_context_node( n);}

void viewer_t::node_added( node_t *n) { view_->node_added( n);}

void viewer_t::node_released( node_t *n)
{
    std::map<node_t*, QWidget*>::iterator it( node_toolbars_.find( n));

    if( it != node_toolbars_.end())
    {
        delete it->second;
        node_toolbars_.erase( n);
    }

    view_->node_released( n);
}

void viewer_t::frame_changed() { view_->frame_changed();}

void viewer_t::set_status( const std::string& text)
{
    status_->setText( QString::fromStdString( text));
}

void viewer_t::begin_interaction() {}
void viewer_t::end_interaction() {}

// slots
void viewer_t::autoupdate_toggle( bool state) { view_->set_autoupdate( state);}

void viewer_t::change_active_context_view( int index)
{
    view_->set_view_mode( ( viewer::viewer_context_t::view_mode_t) index);
}

void viewer_t::change_display_device( int index)
{
    display_device_ = display_devices_[index];

    // refresh the list of transforms
    ocio_transform_combo_->blockSignals( true);
    ocio_transform_combo_->clear();

    OCIO::ConstConfigRcPtr config = app().ocio_manager().config();

    std::string default_transform = config->getDefaultView( display_device_.c_str());
    int num_transform_names = config->getNumViews( display_device_.c_str());

    int default_index = 0, new_index = -1;
    QStringList slist;

    for( int i = 0; i < num_transform_names; ++i)
    {
        std::string trname = config->getView( display_device_.c_str(), i);
        display_transforms_.push_back( trname);
        slist << trname.c_str();

        if( display_transform_ == trname)
            new_index = i;

        if( default_transform == trname)
            default_index = i;
    }

    ocio_transform_combo_->addItems( slist);

    if( new_index < 0)
        new_index = default_index;

    ocio_transform_combo_->setCurrentIndex( new_index);
    display_transform_ = ocio_transform_combo_->itemText( new_index).toStdString();
    ocio_transform_combo_->blockSignals( false);
	view_->display_transform_changed();
}

void viewer_t::change_display_transform( int index)
{
    display_transform_ = display_transforms_[index];
	view_->display_transform_changed();
}

void viewer_t::change_exposure( double d)	{ view_->exposure_changed();}
void viewer_t::change_gamma( double d)		{ view_->gamma_changed();}

} // namespace
} // namespace
