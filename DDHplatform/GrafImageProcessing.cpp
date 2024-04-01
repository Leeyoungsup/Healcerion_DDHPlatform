#include "pch.h"
#include "GrafImageProcessing.h"



double GrafImageProcessing::getOrientation(const vector<Point>& pts, Mat& img ,PCA& pca_analysis)
{
    //Construct a buffer used by the pca analysis
    int sz = static_cast<int>(pts.size());
    Mat data_pts = Mat(sz, 2, CV_64FC1);
    for (int i = 0; i < data_pts.rows; ++i)
    {
        data_pts.at<double>(i, 0) = pts[i].x;
        data_pts.at<double>(i, 1) = pts[i].y;
    }

    //Perform PCA analysis
    pca_analysis(data_pts, Mat(),  PCA::DATA_AS_ROW);
    
    //Store the center of the object
    Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
        static_cast<int>(pca_analysis.mean.at<double>(0, 1)));

    //Store the eigenvalues and eigenvectors
    vector<Point2d> eigen_vecs(2);
    for (int i = 0; i < 2; ++i)
    {
        eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
            pca_analysis.eigenvectors.at<double>(i, 1));
    }

    double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians

    return angle;
}