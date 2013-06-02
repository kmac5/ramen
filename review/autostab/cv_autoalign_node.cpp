// Copyright (c) 2010 Esteban Tovagliari

#include<ramen/nodes/image/track/cv_autoalign_node.hpp>

#include<algorithm>

#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/calib3d/calib3d.hpp>

#include<ramen/assert.hpp>

#include<ramen/render/image_node_renderer.hpp>

#include<ramen/image/opencv.hpp>

namespace ramen
{
namespace
{

struct order_keypoints_greater
{
	bool operator()( const cv::KeyPoint& k0, const cv::KeyPoint& k1) const
	{
		return k0.response > k1.response;
	}
};

struct order_keypoints_less
{
	bool operator()( const cv::KeyPoint& k0, const cv::KeyPoint& k1) const
	{
		return k0.response < k1.response;
	}
};

} // unnamed

cv_autoalign_node_t::cv_autoalign_node_t() : autoalign_base_node_t() {}
cv_autoalign_node_t::cv_autoalign_node_t( const cv_autoalign_node_t& other) : autoalign_base_node_t( other) {}

// matching

// features
void cv_autoalign_node_t::detect_surf_features( const cv::Mat& img, const cv::Mat& msk, std::vector<cv::KeyPoint>& feat, int max_features)
{
	cv::SURF surf;
	surf( img, msk, feat);

	if( max_features > 0)
		limit_num_features( feat, max_features);
}

void cv_autoalign_node_t::limit_num_features( std::vector<cv::KeyPoint>& feat, int max_features)
{
	if( feat.size() > max_features)
	{
		std::sort( feat.begin(), feat.end(), order_keypoints_greater());
		feat.erase( feat.begin() + max_features - 1, feat.end());
	}
}

// descriptors
void cv_autoalign_node_t::extract_surf_descriptors( const cv::Mat& img, std::vector<cv::KeyPoint>& feat, cv::Mat& descriptors)
{
	cv::Ptr<cv::DescriptorExtractor> d = cv::DescriptorExtractor::create( "SURF");
	d->compute( img, feat, descriptors);
}

// matching
void cv_autoalign_node_t::cross_check_matching( cv::Ptr<cv::DescriptorMatcher>& descriptorMatcher,
						   const cv::Mat& descriptors1, const cv::Mat& descriptors2,
						   std::vector<cv::DMatch>& filteredMatches12, int knn)
{
    filteredMatches12.clear();
    std::vector<std::vector<cv::DMatch> > matches12, matches21;
    descriptorMatcher->knnMatch( descriptors1, descriptors2, matches12, knn);
    descriptorMatcher->knnMatch( descriptors2, descriptors1, matches21, knn);
	
    for( size_t m = 0; m < matches12.size(); m++)
    {
        bool findCrossCheck = false;
        for( size_t fk = 0; fk < matches12[m].size(); fk++)
        {
            cv::DMatch forward = matches12[m][fk];

            for( size_t bk = 0; bk < matches21[forward.trainIdx].size(); bk++)
            {
                cv::DMatch backward = matches21[forward.trainIdx][bk];

                if( backward.trainIdx == forward.queryIdx )
                {
                    filteredMatches12.push_back(forward);
                    findCrossCheck = true;
                    break;
                }
            }
			
            if( findCrossCheck )
				break;
        }
    }
}

boost::optional<Imath::M33d> cv_autoalign_node_t::find_similarity( const std::vector<cv::Point2d>& from_pts, const std::vector<cv::Point2d>& to_pts)
{	
	cv::Mat from( 1, from_pts.size(), CV_32FC2);
	cv::Mat to( 1, from_pts.size(), CV_32FC2);

	float *pf = from.ptr<float>( 0);
	float *pt = to.ptr<float>( 0);
	
	for( int i = 0; i < from_pts.size(); ++i)
	{
		*pf++ = from_pts[i].x;
		*pf++ = from_pts[i].y;
		*pt++ = to_pts[i].x;
		*pt++ = to_pts[i].y;
	}
	
	cv::Mat h = cv::estimateRigidTransform( from, to, false);
	Imath::M33d m;
	
	for( int j = 0; j < 3; ++j)
		for( int i = 0; i < 3; ++i)
			m[j][i] = h.at<double>( i, j);
	
	return m;
}

boost::optional<Imath::M33d> cv_autoalign_node_t::find_homography( const std::vector<cv::Point2d>& from_pts, const std::vector<cv::Point2d>& to_pts)
{
	cv::Mat h = cv::findHomography( cv::Mat( from_pts), cv::Mat( to_pts), CV_RANSAC);
	Imath::M33d m;
	
	for( int j = 0; j < 3; ++j)
		for( int i = 0; i < 3; ++i)
			m[j][i] = h.at<double>( i, j);
	
	return m;
}

// rendering
void cv_autoalign_node_t::get_frame( const render::context_t& context, cv::Mat& dst, cv::Mat& msk, int use_alpha)
{
	image_node_t *inode = dynamic_cast<image_node_t*>( context.result_node);
	RAMEN_ASSERT( inode);
	
	render::image_node_renderer_t r( context);
	r.render();
		
	int dst_w = inode->format().size().x + 1;
	int dst_h = inode->format().size().y + 1;
	
	if( dst.type() != CV_8U || ( dst.cols != dst_w) || ( dst.rows != dst_h))
		dst = cv::Mat( dst_h, dst_w, CV_8U);

	if( use_alpha)
	{
		if( msk.type() != CV_8U || ( msk.cols != dst_w) || ( msk.rows != dst_h))
			msk = cv::Mat( dst_h, dst_w, CV_8U);		
	}
	
	Imath::Box2i area = intersect( inode->format(), inode->defined());
	
	if( !area.isEmpty())
	{
		Imath::V2i offset = inode->defined().min - inode->format().min;
		RAMEN_ASSERT( offset.x >= 0 && offset.y >= 0);
		image::rgba_subimage_to_cvmat_8u( inode->const_subimage_view( area), dst, offset.x, offset.y);
		
		if( use_alpha)
			image::alpha_subimage_to_cvmat_8u( inode->const_subimage_view( area), msk, offset.x, offset.y);
	}
	else
	{
		// clear with zeros.
		for( int j = 0; j < dst.rows; ++j)
		{
			unsigned char *dst_it = dst.ptr<unsigned char>( j);
			for( int i = 0; i < dst.cols; ++i)
				*dst_it++ = 0;
		}
		
		if( use_alpha)
		{
			for( int j = 0; j < msk.rows; ++j)
			{
				unsigned char *msk_it = msk.ptr<unsigned char>( j);
				for( int i = 0; i < msk.cols; ++i)
					*msk_it++ = 0;
			}
		}
	}
	
	if( use_alpha == 2)
		image::invert_cvmat_8u( msk);
}

} // namespace
