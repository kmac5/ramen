// Copyright (c) 2011 Esteban Tovagliari

#ifndef RAMEN_KEYER3D_TOOLBAR_HPP
#define	RAMEN_KEYER3D_TOOLBAR_HPP

#include<memory>

#include<QWidget>

#include<ramen/nodes/image/key/keyer3d/keyer3d_node_fwd.hpp>
#include<ramen/nodes/image/key/keyer3d/tool_fwd.hpp>

class QToolButton;

namespace ramen
{
namespace keyer3d
{

class toolbar_t : public QWidget
{
    Q_OBJECT
    
public:

    toolbar_t( image::keyer3d_node_t& parent);
	virtual ~toolbar_t();
	
	const tool_t *active_tool() const;
	tool_t *active_tool();

public Q_SLOTS:

    void pick_sample_tool( bool b);
    void pick_tolerance_tool( bool b);
    void pick_softness_tool( bool b);

private:

    image::keyer3d_node_t& parent_;
		
	tool_t *sample_tool_;
    tool_t *tolerance_tool_;
    tool_t *softness_tool_;

    tool_t *current_tool_;
	
	QToolButton *sample_;
	QToolButton *tolerance_;
	QToolButton *softness_;
};

} // namespace
} // namespace

#endif
