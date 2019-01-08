#include "opencv2/opencv.hpp"
#include <bits/stdc++.h>
#include <GL/glut.h>
using namespace std;
using namespace cv;

bool comp (DMatch d, DMatch d2) {
	return d.distance < d2.distance;
}

int main( int, char** ) {

	namedWindow("Match", WINDOW_AUTOSIZE);
	Mat image1 = imread("Dtone.jpeg", IMREAD_GRAYSCALE);
	if (image1.empty()) {
		cout << "Image1 vazia\n";
		return -1;
	}
	VideoCapture cap(0);
	if (!cap.isOpened()) {
		return -1;
	}
	Mat image2, img;

	for (;;) {
		cap >> img;
		if (img.empty()) {
			cout << "Image2 vazia\n";
			return -1;
		}
		cvtColor(img, image2, CV_BGR2GRAY);


		int minHessian = 400;
		vector<KeyPoint> kp1, kp2;
		Mat descriptor1, descriptor2;


		Ptr<Feature2D> orb = ORB::create(minHessian);
		orb->detectAndCompute(image1, Mat(), kp1, descriptor1);
		orb->detectAndCompute(image2, Mat(), kp2, descriptor2);


		if ( descriptor1.empty() ) {
			cvError(0,"MatchFinder","1st descriptor empty",__FILE__,__LINE__);    
			return -1;
		}
		if ( descriptor2.empty() ) {
			cvError(0,"MatchFinder","2nd descriptor empty",__FILE__,__LINE__);
			return -1;
		}



		descriptor1.convertTo(descriptor1, CV_32F); descriptor2.convertTo(descriptor2, CV_32F);

		
		vector<DMatch> matches;
		FlannBasedMatcher matcher;
		matcher.match(descriptor1, descriptor2, matches);

		vector<DMatch> GMatches;
		double min = 100, max = 0;

		for (int i = 0; i < desc; i ++) {
			if (matches[i].distance < min) {
				min = matches[i].distance;
			}
			if (matches[i].distance > max) {
				max = matches[i].distance;
			}
		}

		for (int i = 0; i < desc; i ++) {
			if ((matches[i].distance < 3 * min)) {
				GMatches.push_back(matches[i]);
			}
		}

		//long int s =  matches.size();
		//sort(matches.begin(), matches.end(), comp);

		//Desenha as linhas entre os keypoints em imageMatch

		Mat imageMatch;
		drawMatches(image1, kp1, image2, kp2, GMatches, imageMatch, Scalar::all(-1), Scalar::all(-1), 
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); 

		/*imshow("image2", imageMatch);
		waitKey(0);
		destroyAllWindows();*/



		vector<Point2f> staticImage;
		vector<Point2f> videoImage;

		for (int i = 0; i < GMatches.size(); i ++) {
			staticImage.push_back(kp1[GMatches[i].queryIdx].pt);
			videoImage.push_back(kp2[GMatches[i].trainIdx].pt);
		}
		Mat homog = findHomography(staticImage, videoImage, CV_RANSAC);

		vector<Point2f> corner(4);

		corner[0] = cvPoint(0, 0);
		corner[1] = cvPoint(image1.cols, 0);
		corner[2] = cvPoint(image1.cols, image1.rows);
		corner[3] = cvPoint(0, image1.rows);
		
		vector<Point2f> videoCorner(4);	
		perspectiveTransform(corner, videoCorner, homog);


		line(imageMatch, videoCorner[0] + Point2f(image1.cols, 0),
		videoCorner[1] + Point2f(image1.cols, 0), Scalar(0, 255, 0), 4);
		line(imageMatch, videoCorner[1] + Point2f(image1.cols, 0),
		videoCorner[2] + Point2f(image1.cols, 0), Scalar(0, 255, 0), 4);
		line(imageMatch, videoCorner[2] + Point2f(image1.cols, 0),
		videoCorner[3] + Point2f(image1.cols, 0), Scalar(0, 255, 0), 4);
		line(imageMatch, videoCorner[3] + Point2f(image1.cols, 0),
		videoCorner[0] + Point2f(image1.cols, 0), Scalar(0, 255, 0), 4);

		imshow("Match", imageMatch);
		if (waitKey(33) == 27) {
			break;
		}
		
	} 
	destroyAllWindows();
	return 0;
}