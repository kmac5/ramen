// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UI_PY_CONSOLE_HPP
#define	RAMEN_UI_PY_CONSOLE_HPP

#include<boost/noncopyable.hpp>

#include<QObject>

class QWidget;
class QPlainTextEdit;

namespace ramen
{
namespace ui
{
namespace python
{

class console_t : public QObject, boost::noncopyable
{
    Q_OBJECT

public:

    static console_t& instance();

    QWidget *widget() { return window_;}

    QPlainTextEdit *output_area()	{ return output_;}

    void write( const std::string& str);

public Q_SLOTS:

    void clear();

private:

    console_t();
    ~console_t();

    QWidget *window_;
    QPlainTextEdit *output_;
};

} // namespace
} // namespace
} // namespace

#endif
