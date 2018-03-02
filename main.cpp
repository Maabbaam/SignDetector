#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define WARPED_XSIZE 200
#define WARPED_YSIZE 300

using namespace cv;
using namespace std;

Mat src; 
Mat src_gray;
Mat warped_result;
Mat speed_80;
Mat speed_40;
int canny_thresh = 120;
RNG rng(12345);


#define VERY_LARGE_VALUE 100000

#define NO_MATCH    0
#define STOP_SIGN            1
#define SPEED_LIMIT_40_SIGN  2
#define SPEED_LIMIT_80_SIGN  3

static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}

void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}

/** @function main */
int main(int argc, char** argv)
{
	int sign_recog_result = NO_MATCH;
	speed_40 = imread("speed_40.bmp", 0);
	speed_80 = imread("speed_80.bmp", 0);

	// you run your program on these three examples (uncomment the two lines below)
	//string sign_name = "stop4";
	 //string sign_name = "stop_sign";
	 string sign_name = "oct";
	//string sign_name = "speedsign12";
	//string sign_name = "speedsign13";
	//string sign_name = "speedsign14";
	//string sign_name = "speedsign3";
	//string sign_name = "speedsign4";
	//string sign_name = "speedsign5";
	string final_sign_input_name = sign_name + ".jpg";
	string final_sign_output_name = sign_name + "_result" + ".jpg";

	/// Load source image and convert it to gray
	src = imread(final_sign_input_name, 1);

	vector<vector<Point> > contours;
	vector<Mat> matList;

	vector<Vec4i> hierarchy;
	int cannyThread = 120;

	Mat sclo;
	Mat sclu;
	Mat timg;

	//pyrDown(src, sclo, Size(src.cols / 2, src.rows / 2));
	//pyrUp(sclo, timg, src.size());

		Mat output;
		cv::Canny(src, output, cannyThread, cannyThread*2, 3);

		Mat  c = Mat::zeros(output.size(), CV_8UC3);

		Mat  polyD = Mat::zeros(output.size(), CV_8UC3);
		cv::findContours(output, contours,hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));
		Mat outputCurve;
		vector<Point> po;
		
		//Mat con = Mat(contours);

		cv::Mat dst = polyD.clone();

		vector<vector<Point> > upgradedContours;
		Mat up = Mat::zeros(output.size(), CV_8UC3);
		int numberOfOctAngles = 0;
		Mat outputC;

		for (int i = 0; i < contours.size(); i++) {

			
			vector<Point> point = contours.at(i);
		//	Mat outputC;
			Mat input = Mat(contours.at(i));


			cv::approxPolyDP(Mat(contours[i]), outputC, contours[i].size()*.008, true);
			cv::approxPolyDP(Mat(contours[i]), po, contours[i].size()*.008, true);

			

			if (po.size() == 8) {

				setLabel(dst, "OCt", contours[i]);    // stop Sign
				Scalar color = Scalar(rng.uniform(0, 1), rng.uniform(0, 1), rng.uniform(0, 1));
			//	drawContours(dst, contours[i], i, color, 1, 8, hierarchy, 0);
				upgradedContours.push_back(po);

			}

			if (po.size() > 8) {

			
				double cosAngle;
				for (int j = 2; j < po.size() + 1; j++)
				{
					
					double cosine = fabs(angle(po[j % po.size()], po[j - 2], po[j - 1]));
					//cout << " cosAngle between points" << cosine << endl;
					if (cosine > 0.69 && cosine < 0.71) numberOfOctAngles++;
					
				//	cout << "Number of OctAngles"  << numberOfOctAngles << endl;
				}

			}

			//imshow("oct", dst);
			cout << outputC.size() << endl;
			matList.push_back(outputC);

			outputC.release();
			
			//


		}
		

		cout << "Number of OctAngles" << numberOfOctAngles << endl;
	
		
		for (int i = 0; i < contours.size(); i++) {
    Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));


    drawContours(polyD, matList, i, color, 1, 8, hierarchy, 0);
			}
		

		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(c, contours, i, color, 1, 8, hierarchy, 0, Point());
		

		}

		for (int i = 0; i < upgradedContours.size(); i++) {
			Scalar color = Scalar(rng.uniform(254, 255), rng.uniform(254, 255), rng.uniform(254, 255));




			drawContours(c, upgradedContours, i, color, 1, 8, hierarchy, 0);
		}
	//	cv::approxPolyDP(output, outputCurve, contours[1].size()*.008, true);


		//imshow("approx", matList.at(1));


		/*for (let i = 0; i < contours.size(); ++i) {
    let tmp = new cv.Mat();
    let cnt = contours.get(i);
    // You can try more different parameters
    cv.approxPolyDP(cnt, tmp, 3, true);
    poly.push_back(tmp);
    cnt.delete(); tmp.delete();



}*/


		int canny = 200;
		Mat cOut, cnOut;

		vector<vector<Point> > conts;
		vector<Vec4i> hierarchy2;


		cv::Canny(c, cOut, canny, canny * 2, 3);

//		Mat  polyD = Mat::zeros(output.size(), CV_8UC3);
	//	cv::findContours(cOut, conts, hierarchy2, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//	Mat outputCurve;


		//imshow("OutPutC", outputC);

	//	imshow("NewCOUT", cOut);


	//	imshow("Upgrade", up);

		imshow("PolyD", polyD);

	imshow("canny" , output);

	imshow("Countirs", c);

	/// Convert image to gray and blur it
	cvtColor(src, src_gray, COLOR_BGR2GRAY);
	blur(src_gray, src_gray, Size(5, 5));
	warped_result = Mat(Size(WARPED_XSIZE, WARPED_YSIZE), src_gray.type());



	// here you add the code to do the recognition, and set the variable 
	// sign_recog_result to one of STOP_SIGN, SPEED_LIMIT_40_SIGN, SPEED_LIMIT_80_SIGN, or NO_MATCH

	string text;
	if (sign_recog_result == SPEED_LIMIT_40_SIGN) text = "Speed 40";
	else if (sign_recog_result == SPEED_LIMIT_80_SIGN) text = "Speed 80";
	else if (sign_recog_result == STOP_SIGN) text = "Stop";
	else if (sign_recog_result == NO_MATCH) text = "Fail";

	//font 
	int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
	double fontScale = 2;
	int thickness = 3;
	cv::Point textOrg(10, 130);
	cv::putText(src, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);

	/// Create Window
	const char* source_window = "Result";
	namedWindow(source_window, WINDOW_AUTOSIZE);
	imshow(source_window, src);
	imwrite(final_sign_output_name, src);

	waitKey(0);

	return(0);
}
