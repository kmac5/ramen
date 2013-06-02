// Copyright (c) 2010 Esteban Tovagliari

#ifndef RAMEN_IMAGE_CV_AUTOALIGN_NODE_HPP
#define RAMEN_IMAGE_CV_AUTOALIGN_NODE_HPP

#include<ramen/nodes/image/track/autoalign_base_node.hpp>

#include<opencv2/core/core.hpp>
#include<opencv2/features2d/features2d.hpp>

namespace ramen
{

class cv_autoalign_node_t : public autoalign_base_node_t
{
public:

    cv_autoalign_node_t();

protected:

    cv_autoalign_node_t( const cv_autoalign_node_t& other);
    void operator=( const cv_autoalign_node_t&);
	
	// features
	void detect_surf_features( const cv::Mat& img, const cv::Mat& msk, std::vector<cv::KeyPoint>& feat, int max_features = 0);
	
	void limit_num_features( std::vector<cv::KeyPoint>& feat, int max_features);
	
	// descriptors
	void extract_surf_descriptors( const cv::Mat& img, std::vector<cv::KeyPoint>& feat, cv::Mat& descriptors);

	// matching
	void cross_check_matching( cv::Ptr<cv::DescriptorMatcher>& descriptorMatcher,
							   const cv::Mat& descriptors1, const cv::Mat& descriptors2,
							   std::vector<cv::DMatch>& filteredMatches12, int knn=1);
	
	// align
	boost::optional<Imath::M33d> find_similarity( const std::vector<cv::Point2d>& from_pts, const std::vector<cv::Point2d>& to_pts);	
	boost::optional<Imath::M33d> find_homography( const std::vector<cv::Point2d>& from_pts, const std::vector<cv::Point2d>& to_pts);

	// rendering
	void get_frame( const render::context_t& context, cv::Mat& dst, cv::Mat& mask, int use_alpha = 0);

private:
	
};

} // namespace

#endif
