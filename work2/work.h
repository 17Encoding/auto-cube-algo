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
	Point((dst.cols - sz.width) / 2 - sz.width - jiange, (dst.rows - sz.height) / 2 - sz.height - jiange),//���Ͻ�
	Point((dst.cols - sz.width) / 2, (dst.rows - sz.height) / 2 - sz.height - jiange),//�ϱ�
	Point((dst.cols + sz.width) / 2 + jiange, (dst.rows - sz.height) / 2 - sz.height - jiange),//���Ͻ�
	Point((dst.cols - sz.width) / 2 - sz.width - jiange, (dst.rows - sz.height) / 2),//���
	Point((dst.cols - sz.width) / 2, (dst.rows - sz.height) / 2),//����
	Point((dst.cols + sz.width) / 2 + jiange, (dst.rows - sz.height) / 2),//�ұ�
	Point((dst.cols - sz.width) / 2 - sz.width - jiange, (dst.rows + sz.height) / 2 + jiange),//���½�
	Point((dst.cols - sz.width) / 2, (dst.rows + sz.height) / 2 + jiange),//�±�
	Point((dst.cols + sz.width) / 2 + jiange, (dst.rows + sz.height) / 2 + jiange)//���½�
};

char MaxColor(int red, int orange, int yellow, int white, int green, int blue);
void preparation(Mat& src, Mat& dst);
void rectMat(Mat& frame);
void build_data(Mat sample_hsv, float sample_data[][3]);
void capture(Mat& sample_src);
void test();

#endif // !_WORK_H_
