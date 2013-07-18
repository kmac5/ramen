// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_UI_PY_EDITOR_HPP
#define	RAMEN_UI_PY_EDITOR_HPP

#include<boost/noncopyable.hpp>

#include<QObject>

class QPoint;
class QWidget;
class QPlainTextEdit;

namespace ramen
{
namespace ui
{
namespace python
{

class editor_t : public QObject, boost::noncopyable
{
    Q_OBJECT

public:

    static editor_t& instance();

    QWidget *widget() { return window_;}

    QPlainTextEdit *edit_area()	{ return edit_;}

private Q_SLOTS:

    void run();
	void show_context_menu( const QPoint& p);

private:

    editor_t();
    ~editor_t();

    QWidget *window_;
    QPlainTextEdit *edit_;
};

} // namespace
} // namespace
} // namespace

#endif
