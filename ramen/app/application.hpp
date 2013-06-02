// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_APP_APPLICATION_HPP
#define RAMEN_APP_APPLICATION_HPP

#include<ramen/config.hpp>

#include<ramen/app/application_fwd.hpp>

#include<string>
#include<memory>

#include<boost/noncopyable.hpp>
#include<boost/cstdint.hpp>

#include<tbb/task_scheduler_init.h>

#include<ramen/system/system.hpp>

#include<ramen/app/preferences.hpp>
#include<ramen/app/document_fwd.hpp>

#include<ramen/memory/manager_fwd.hpp>
#include<ramen/render/render_thread.hpp>
#include<ramen/ocio/manager_fwd.hpp>

#include<ramen/filesystem/path.hpp>

#include<ramen/util/command_line_parser_fwd.hpp>

#include<ramen/ui/user_interface_fwd.hpp>
#include<ramen/ui/dialogs/splash_screen_fwd.hpp>

namespace ramen
{

/*!
\ingroup app
\brief Application class.
*/
class RAMEN_API application_t : boost::noncopyable
{
public:

	application_t( int argc, char **argv);
	~application_t();

	int run();

	bool command_line() const { return command_line_;}

	int max_threads() const { return max_threads_;}

	// messages
	void fatal_error( const std::string& message, bool no_gui = false) const;
	void error( const std::string& message, bool no_gui = false) const;
	void inform( const std::string& message, bool no_gui = false) const;
	bool question( const std::string& what, bool default_answer = true) const;

	const system::system_t& system() { return system_;}

	const preferences_t& preferences() const    { return *preferences_;}
	preferences_t& preferences()                { return *preferences_;}

	const memory::manager_t& memory_manager() const { return *mem_manager_;}
	memory::manager_t& memory_manager()             { return *mem_manager_;}

	render::render_thread_t& render_thread() { return render_thread_;}

	// opencolorio
	const ocio::manager_t& ocio_manager() const { return *ocio_manager_;}
	ocio::manager_t& ocio_manager()             { return *ocio_manager_;}

	// user interface
	const ui::user_interface_t *ui() const  { return ui_.get();}
	ui::user_interface_t *ui()              { return ui_.get();}

	// document handling
	const document_t& document() const  { return *document_;}
	document_t& document()              { return *document_;}

	void create_new_document();
	void open_document( const boost::filesystem::path& p);
	void delete_document();

	bool quitting() const       { return quitting_;}
	void set_quitting( bool b)  { quitting_ = b;}

private:

	void create_dirs();

	// command line
	bool matches_option( char *arg, const char *opt) const;
	boost::optional<int> parse_int( int num, int argc, char **argv) const;
	boost::optional<float> parse_float( int num, int argc, char **argv) const;
	void parse_input_file( char *arg);
	bool parse_common_option( int argc, char **argv, int& num);

	void parse_command_line( int argc, char **argv);
	void parse_render_command_line( int argc, char **argv, int num);

	void usage();
	void render_usage();

	void print_app_info();

	// opencolorio
	void init_ocio();
	bool init_ocio_config_from_file( const boost::filesystem::path& p);

	// data
    std::auto_ptr<util::command_line_parser_t> cmd_parser_;
	boost::uint64_t img_cache_size_;
	int max_threads_;
	bool command_line_;
	boost::filesystem::path infile_;
	bool render_mode_;

	tbb::task_scheduler_init task_scheduler_;
	system::system_t system_;
	std::auto_ptr<preferences_t> preferences_;
	std::auto_ptr<memory::manager_t> mem_manager_;
	render::render_thread_t render_thread_;
	std::auto_ptr<ocio::manager_t> ocio_manager_;
	std::auto_ptr<ui::user_interface_t> ui_;

	std::auto_ptr<document_t> document_;

	boost::optional<int> start_frame_, end_frame_, proxy_level_,
						subsample_, mb_extra_samples_;

	boost::optional<float> mb_shutter_factor_;

	std::auto_ptr<ui::splash_screen_t> splash_;

	bool quitting_;

    #ifndef NDEBUG
        bool run_unit_tests_;
    #endif
};

RAMEN_API application_t& app();

} // namespace

#endif
