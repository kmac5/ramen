// Copyright (c) 2011 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_FLIPBOOK_EXTERNAL_FLIPBOOK_HPP
#define RAMEN_FLIPBOOK_EXTERNAL_FLIPBOOK_HPP

#include<ramen/flipbook/flipbook.hpp>
#include<QObject>

#include<vector>

#include<boost/lexical_cast.hpp>

#include<QProcess>
#include<QProgressDialog>

#include<ramen/filesystem/path.hpp>

namespace ramen
{
namespace flipbook
{

class external_flipbook_t : public QObject, public flipbook_t
{
	Q_OBJECT
	
protected:

    external_flipbook_t( int frame_rate, const std::string& display_device, const std::string& display_transform);
	virtual ~external_flipbook_t();

	virtual void begin_progress();
	virtual void end_progress();
	
	virtual void add_frame( int frame, image::buffer_t pixels);
	
	virtual void save_frame( int frame) const;
	
	virtual void play();
	
	virtual std::string program() const = 0;
	virtual void arguments( std::vector<std::string>& args) const = 0;
	
private Q_SLOTS:

	void process_error( QProcess::ProcessError);
	void process_exited( int status);
	
protected:
	
	boost::filesystem::path path_for_frame( int num) const;
		
	QProcess *proc_;
	QProgressDialog *progress_;
	
	bool empty_;
	
	boost::filesystem::path dir_;
	std::string fname_;
	std::string ext_;
	bool use_data_window_;
};

} // namespace
} // namespace

#endif
