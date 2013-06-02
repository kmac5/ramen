// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/widgets/label_separator.hpp>

#include<QFrame>
#include<QHBoxLayout>

namespace ramen
{
namespace ui
{

label_separator_t::label_separator_t( QWidget *parent) : QWidget( parent)
{
    label_ = new QLabel();
    init();
}

label_separator_t::label_separator_t( const QString& text, QWidget *parent) : QWidget( parent)
{
    label_ = new QLabel( text);
    init();
}

void label_separator_t::init()
{
    QHBoxLayout *hlayout = new QHBoxLayout( this);

    QSizePolicy sizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth( label_->sizePolicy().hasHeightForWidth());
    label_->setSizePolicy( sizePolicy);
    hlayout->addWidget( label_);

    QFrame *line = new QFrame();
    line->setFrameShape( QFrame::HLine);
    line->setFrameShadow( QFrame::Sunken);
    hlayout->addWidget( line);
}

} // ui
} // ramen
