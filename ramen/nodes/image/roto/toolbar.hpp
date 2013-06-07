// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_ROTO_TOOLBAR_HPP
#define	RAMEN_ROTO_TOOLBAR_HPP

#include<memory>

#include<QWidget>

#include<ramen/nodes/image/roto/roto_node_fwd.hpp>
#include<ramen/nodes/image/roto/tool_fwd.hpp>

namespace ramen
{
namespace roto
{

class toolbar_t : public QWidget
{
    Q_OBJECT
    
public:

    toolbar_t( image::roto_node_t& parent);
	virtual ~toolbar_t();
	
	const tool_t& active_tool() const;
	tool_t& active_tool();
	
public Q_SLOTS:

    void pick_select_tool( bool b);
    void pick_create_tool( bool b);
    void pick_rect_tool( bool b);
    void pick_null_tool( bool b);
    void pick_convert_tool( bool b);
	
private:

    image::roto_node_t& parent_;
	
    tool_t *select_tool_;
    tool_t *create_tool_;
	tool_t *rect_tool_;
	tool_t *null_tool_;
    tool_t *convert_tool_;
    tool_t *current_tool_;
};

} // namespace
} // namespace

#endif
