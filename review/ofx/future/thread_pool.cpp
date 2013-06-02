// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/render/render_thread.hpp>

#include<boost/bind.hpp>

#include<ramen/assert.hpp>

namespace ramen
{
namespace render
{

struct render_thread_t::implementation
{
    implementation()
    {
        // start our thread
        finish = false;
        ready = false;
        do_work = false;
        thread = boost::thread( &implementation::thread_loop, this);
    }

    ~implementation()
    {
        boost::unique_lock<boost::mutex> lock( mutex);
        finish = true;
        ready = true;
        do_work = false;
        lock.unlock();
        cond.notify_one();

        // wait for our thread to exit
        thread.join();
    }

    boost::unique_future<bool>& run_function( const boost::function<void()>& f)
    {
        RAMEN_ASSERT( !do_work && "render_thread_t is not reentrant");
        boost::unique_lock<boost::mutex> lock( mutex);
        cancel = false;
        ready = true;
        do_work = true;
        task = boost::packaged_task<bool>( boost::bind( &implementation::call_fun, this, f));
        future = task.get_future();
        lock.unlock();
        cond.notify_one();
        return future;
    }

private:

    void thread_loop()
    {
        while( 1)
        {
            boost::unique_lock<boost::mutex> lock( mutex);

            while( !ready)
                cond.wait( lock);

            if( finish)
                break;

            if( do_work)
            {
                do_work = false;
                ready = false;
                task();
            }
        }
    }

    bool call_fun( const boost::function<void()>& f)
    {
        f();
        return !cancel;
    }

public:

    boost::thread thread;
    bool finish;
    bool ready;
    bool do_work;
    bool cancel;

    boost::mutex mutex;
    boost::condition_variable cond;

    boost::packaged_task<bool> task;
    boost::unique_future<bool> future;    
};

render_thread_t::render_thread_t() : pimpl_( 0) {}
render_thread_t::~render_thread_t() { delete pimpl_;}

void render_thread_t::init()
{
    RAMEN_ASSERT( pimpl_ == 0 && "render_thread_t: init called twice");
    pimpl_ = new implementation();
}

boost::unique_future<bool>& render_thread_t::render_image( image_node_renderer_t& renderer)
{
    return render_image( renderer, renderer.format());
}

boost::unique_future<bool>& render_thread_t::render_image( image_node_renderer_t& renderer, const Imath::Box2i& roi)
{
    return pimpl_->run_function( boost::bind( &image_node_renderer_t::render, boost::ref( renderer), roi));
}

bool render_thread_t::cancelled() const { return pimpl_->cancel;}

void render_thread_t::cancel_render() { pimpl_->cancel = true;}

} // namespace
} // namespace
