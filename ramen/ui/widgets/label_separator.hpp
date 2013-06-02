// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_UI_LABELSEPARATOR_HPP
#define	RAMEN_UI_LABELSEPARATOR_HPP

#include<QWidget>
#include<QLabel>

namespace ramen
{
namespace ui
{

class label_separator_t : public QWidget
{
    Q_OBJECT

public:

    label_separator_t( QWidget * parent = 0);
    label_separator_t( const QString & text, QWidget * parent = 0);

private:

    void init();

    QLabel *label_;
};

} // ui
} // ramen

#endif
