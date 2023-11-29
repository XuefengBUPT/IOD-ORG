#include "stdio.h"
#include "mex.h"
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"

#define uint8 unsigned char

using namespace cv;
using namespace std;

void find_feature_matches(
    const Mat &img_1, const Mat &img_2,
    std::vector<KeyPoint> &keypoints_1,
    std::vector<KeyPoint> &keypoints_2,
    std::vector<DMatch> &matches);

void pose_estimation_2d2d(Mat &K,
    std::vector<KeyPoint> keypoints_1,
    std::vector<KeyPoint> keypoints_2,
    std::vector<DMatch> matches,
    Mat &R, Mat &t);

void find_feature_matches(const Mat &img_1, const Mat &img_2,
                          std::vector<KeyPoint> &keypoints_1,
                          std::vector<KeyPoint> &keypoints_2,
                          std::vector<DMatch> &matches)
{
    //-- ��ʼ��
    Mat descriptors_1, descriptors_2;
    // used in OpenCV3
    Ptr<FeatureDetector> detector = ORB::create();
    Ptr<DescriptorExtractor> descriptor = ORB::create();
    // Ptr<FeatureDetector> detector = FeatureDetector::create ( "ORB" );
    // Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create ( "ORB" );
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce-Hamming");
    //-- ��һ��:��� Oriented FAST �ǵ�λ��
    detector->detect(img_1, keypoints_1);
    detector->detect(img_2, keypoints_2);

    //-- �ڶ���:���ݽǵ�λ�ü��� BRIEF ������
    descriptor->compute(img_1, keypoints_1, descriptors_1);
    descriptor->compute(img_2, keypoints_2, descriptors_2);

    //-- ������:������ͼ���е�BRIEF�����ӽ���ƥ�䣬ʹ�� Hamming ����
    vector<DMatch> match;
    //BFMatcher matcher ( NORM_HAMMING );
    matcher->match(descriptors_1, descriptors_2, match);

    //-- ���Ĳ�:ƥ����ɸѡ
    double min_dist = 10000, max_dist = 0;

    //�ҳ�����ƥ��֮�����С�����������, ���������Ƶĺ�����Ƶ������֮��ľ���
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        double dist = match[i].distance;
        if (dist < min_dist)
            min_dist = dist;
        if (dist > max_dist)
            max_dist = dist;
    }
    //printf("-- point num : %d \n", descriptors_1.rows);
    //printf("-- Max dist : %f \n", max_dist);
    //printf("-- Min dist : %f \n", min_dist);

    //��������֮��ľ��������������С����ʱ,����Ϊƥ������.����ʱ����С�����ǳ�С,����һ������ֵ30��Ϊ����.
    for (int i = 0; i < descriptors_1.rows; i++)
    {
        if (match[i].distance <= max(2 * min_dist, 30.0))
        {
            matches.push_back(match[i]);
        }
    }
}

Point2d pixel2cam(const Point2d &p, const Mat &K)
{
    return Point2d(
        (p.x - K.at<double>(0, 2)) / K.at<double>(0, 0),
        (p.y - K.at<double>(1, 2)) / K.at<double>(1, 1));
}

void pose_estimation_2d2d(Mat &K, std::vector<KeyPoint> keypoints_1,
                          std::vector<KeyPoint> keypoints_2,
                          std::vector<DMatch> matches,
                          Mat &R, Mat &t)
{
    //-- ��ƥ���ת��Ϊvector<Point2f>����ʽ
    vector<Point2f> points1;
    vector<Point2f> points2;

    for (int i = 0; i < (int)matches.size(); i++)
    {
        points1.push_back(keypoints_1[matches[i].queryIdx].pt);
        points2.push_back(keypoints_2[matches[i].trainIdx].pt);
    }

    //-- �����������
//     Mat fundamental_matrix;
//     fundamental_matrix = findFundamentalMat(points1, points2, CV_FM_8POINT);
    //cout << "fundamental_matrix is " << endl << fundamental_matrix << endl;

    //-- ���㱾�ʾ���
    Point2d principal_point(325.1, 249.7); //�������, TUM dataset�궨ֵ
    double focal_length = 521;             //�������, TUM dataset�궨ֵ
    Mat essential_matrix;
    essential_matrix = findEssentialMat(points1, points2, focal_length, principal_point);
    //cout << "essential_matrix is " << endl
      //   << essential_matrix << endl;

    //-- ���㵥Ӧ����
//     Mat homography_matrix;
//     homography_matrix = findHomography(points1, points2, RANSAC, 3);
    //cout << "homography_matrix is " << endl
      //   << homography_matrix << endl;

    //-- �ӱ��ʾ����лָ���ת��ƽ����Ϣ.
    recoverPose(essential_matrix, points1, points2, R, t, focal_length, principal_point);
    /*cout << "R is " << endl
         << R << endl;
    cout << "t is " << endl
         << t << endl;*/
}



void mexFunction(int nlhs, mxArray *pl[], int nrhs, const mxArray *pr[])
{
    int c, r;
    uint8 *I1 = (uint8 *)mxGetData(pr[0]);
    uint8 *I2 = (uint8 *)mxGetData(pr[1]);
    double *cameraMat = (double *)mxGetData(pr[2]);
    const mwSize *imgSize = mxGetDimensions(pr[0]);
    const int h = (int)imgSize[0];
    const int w = (int)imgSize[1];
    const int Z = mxGetNumberOfDimensions(pr[1]) <= 2 ? 1 : imgSize[2];

    Mat img_1(h, w, CV_8UC3);
    Mat img_2(h, w, CV_8UC3);
    for (c = 0; c < w; c++)
        for (r = 0; r < h; r++)
            for (int d = 0;d<Z;d++)
            {
                img_1.at<Vec3b>(r, c)[d] = I1[d*h*w + c * h + r];
                img_2.at<Vec3b>(r, c)[d] = I2[d*h*w + c * h + r];
            }
    
    Mat K(3, 3, CV_64FC1);
    for (c = 0; c < 3; c++)
        for (r = 0; r < 3; r++)
            K.at<double>(r, c) = cameraMat[c * 3 + r];

    vector<KeyPoint> keypoints_1, keypoints_2;
    vector<DMatch> matches;
    find_feature_matches(img_1, img_2, keypoints_1, keypoints_2, matches);

    //-- ��������ͼ����˶�
    Mat R, t;
    pose_estimation_2d2d(K, keypoints_1, keypoints_2, matches, R, t);

    mwSize RDims[2] = {3, 3};
    mwSize tDims[2] = {3, 1};
    pl[0] = mxCreateNumericArray(2, RDims, mxDOUBLE_CLASS, mxREAL);
    pl[1] = mxCreateNumericArray(1, tDims, mxDOUBLE_CLASS, mxREAL);
    double *dR = (double *)mxGetData(pl[0]);
    double *dt = (double *)mxGetData(pl[1]);

    for (c = 0; c < 3; c++)
        for (r = 0; r < 3; r++)
            dR[c * 3 + r] = R.at<double>(r, c);
    for (c = 0; c < 3; c++)
        dt[c] = t.at<double>(c, 0);
    return;
}