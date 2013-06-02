// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/python.hpp>

#include<ramen/ui/python/editor.hpp>

#include<boost/python.hpp>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QPlainTextEdit>
#include<QPushButton>

#include<ramen/app/application.hpp>
#include<ramen/app/document.hpp>

#include<ramen/python/interpreter.hpp>

#include<ramen/undo/stack.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/python/console.hpp>

namespace ramen
{
namespace ui
{
namespace python
{

editor_t& editor_t::instance()
{
    static editor_t e;
    return e;
}

editor_t::editor_t() : window_(0)
{
    window_ = new QWidget();
    window_->setWindowTitle( QObject::tr( "Python Editor"));

    QVBoxLayout *layout = new QVBoxLayout();

    edit_ = new QPlainTextEdit();
	edit_->setFont( app().ui()->get_fixed_width_code_font());

	edit_->setContextMenuPolicy( Qt::CustomContextMenu);
	connect( edit_, SIGNAL( customContextMenuRequested( const QPoint&)), this, SLOT( show_context_menu( const QPoint&)));

    layout->addWidget( edit_);

    QWidget *controls_ = new QWidget();

	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->insertStretch( 0);

	QPushButton *button = new QPushButton( "Run");
	connect( button, SIGNAL( clicked()), this, SLOT( run()));
	hlayout->addWidget( button);

    controls_->setLayout( hlayout);
	layout->addWidget( controls_);

    window_->setLayout( layout);
}

editor_t::~editor_t() { window_->deleteLater();}

void editor_t::run()
{
	python::console_t::instance().clear();
    app().document().undo_stack().clear_all();

    QString script = edit_->toPlainText();

    try
    {
		boost::python::handle<> ignored(( PyRun_String( script.toStdString().c_str(), Py_file_input,
														ramen::python::interpreter_t::instance().main_namespace().ptr(),
														ramen::python::interpreter_t::instance().main_namespace().ptr())));

		app().ui()->update();
    }
    catch( boost::python::error_already_set)
    {
		PyErr_Print();
		PyErr_Clear();
    }
}

void editor_t::show_context_menu( const QPoint& p)
{
}

} // namespace
} // namespace
} // namespace
