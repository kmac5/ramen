// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/python/python.hpp>

#include<ramen/ui/python/console.hpp>

#include<boost/python.hpp>

#include<QVBoxLayout>
#include<QHBoxLayout>
#include<QPlainTextEdit>
#include<QPushButton>

#include<ramen/app/application.hpp>

#include<ramen/ui/user_interface.hpp>

#include<ramen/python/interpreter.hpp>

namespace ramen
{
namespace ui
{
namespace python
{
namespace
{

class output_redirector
{
public:

    output_redirector() {}
    void write( const std::string& str) { console_t::instance().write( str);}
};

output_redirector out_to_console;

} // unnamed

console_t& console_t::instance()
{
    static console_t c;
    return c;
}

console_t::console_t() : window_(0)
{
    window_ = new QWidget();
    window_->setWindowTitle( "Python Console");

    QVBoxLayout *layout = new QVBoxLayout();

    output_ = new QPlainTextEdit();
	output_->setFont( app().ui()->get_fixed_width_code_font());
    output_->setReadOnly( true);
    output_->setTabStopWidth( 4);
    layout->addWidget( output_);

    QWidget *controls_ = new QWidget();
	QHBoxLayout *hlayout = new QHBoxLayout();
	hlayout->insertStretch( 0);

	QPushButton *button = new QPushButton( "Clear");
	connect( button, SIGNAL( clicked()), this, SLOT( clear()));
	hlayout->addWidget( button);

    controls_->setLayout( hlayout);
	layout->addWidget( controls_);

    window_->setLayout( layout);

    out_to_console = output_redirector();

    ramen::python::interpreter_t::instance().main_namespace()["output_redirector"] =
	    boost::python::class_<output_redirector>( "output_redirector", boost::python::init<>())
			.def( "write", &output_redirector::write);

    boost::python::import( "sys").attr( "stderr") = out_to_console;
    boost::python::import( "sys").attr( "stdout") = out_to_console;
}

console_t::~console_t() { window_->deleteLater();}

void console_t::clear() { output_area()->clear();}

void console_t::write( const std::string& str)
{
    output_area()->insertPlainText( QString::fromStdString( str));
    output_area()->update();
}

} // namespace
} // namespace
} // namespace
