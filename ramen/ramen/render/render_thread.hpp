// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_RENDER_RENDER_THREAD_HPP
#define RAMEN_RENDER_RENDER_THREAD_HPP

#include<boost/function.hpp>
#include<boost/thread.hpp>

#include<ramen/render/image_node_renderer.hpp>

namespace ramen
{
namespace render
{

struct RAMEN_API render_thread_t
{
    render_thread_t();
    ~render_thread_t();

    void init();

    boost::unique_future<bool>& render_image( image_node_renderer_t& renderer);
    boost::unique_future<bool>& render_image( image_node_renderer_t& renderer, const Imath::Box2i& roi);

    bool cancelled() const;
    void cancel_render();

private:

    struct implementation;
    implementation *pimpl_;
};

} // namespace
} // namespace

#endif
