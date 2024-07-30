#pragma once
#ifndef _WORK_H_
#define _WORK_H_
#include <ml.hpp>
#include <opencv.hpp>

using namespace std;
using namespace cv;
using namespace ml;

const Size2i sz = Size2i(60, 60);
const Size2i sz2 = Size2i(74, 74);
const int jiange = 15;
const Mat dst = imread("dst.jpg");
const int comeraID = 1;
const int commID = 4;

const Point2i cube[9] = {
	Point((dst.cols - sz.width) / 2 - sz.width - jiange, (dst.rows - sz.height) / 2 - sz.height - jiange),//左上角
	Point((dst.cols - sz.width) / 2, (dst.rows - sz.height) / 2 - sz.height - jiange),//上边
	Point((dst.cols + sz.width) / 2 + jiange, (dst.rows - sz.height) / 2 - sz.height - jiange),//右上角
	Point((dst.cols - sz.width) / 2 - sz.width - jiange, (dst.rows - sz.height) / 2),//左边
	Point((dst.cols - sz.width) / 2, (dst.rows - sz.height) / 2),//中心
	Point((dst.cols + sz.width) / 2 + jiange, (dst.rows - sz.height) / 2),//右边
	Point((dst.cols - sz.width) / 2 - sz.width - jiange, (dst.rows + sz.height) / 2 + jiange),//左下角
	Point((dst.cols - sz.width) / 2, (dst.rows + sz.height) / 2 + jiange),//下边
	Point((dst.cols + sz.width) / 2 + jiange, (dst.rows + sz.height) / 2 + jiange)//右下角
};

char MaxColor(int red, int orange, int yellow, int white, int green, int blue);
void preparation(Mat& src, Mat& dst);
void rectMat(Mat& frame);
void build_data(Mat sample_hsv, float sample_data[][3]);
void capture(Mat& sample_src);
void test();

#endif // !_WORK_H_
