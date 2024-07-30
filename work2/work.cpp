#include "work.h"

char MaxColor(int red, int orange, int yellow, int white, int green, int blue) {
	int color[6], max = -100000, imax = -1;
	color[0] = red, color[1] = orange, color[2] = yellow, color[3] = white, color[4] = green, color[5] = blue;
	for (int i = 0; i < 6; i++)
		if (color[i] > max) {
			max = color[i];
			imax = i;
		}

	switch (imax) {
	case 0:return 'R';
	case 1:return 'O';
	case 2:return 'Y';
	case 3:return 'W';
	case 4:return 'G';
	case 5:return 'B';
	}
}

void preparation(Mat& src, Mat& dst) {
	Mat bal;
	vector<Mat> src_split, dst_split;
	//第一步：高斯模糊
	GaussianBlur(src, bal, Size(3, 3), 3, 3);

	//第二步：白平衡
	split(bal, src_split);

	double B = mean(src_split[0])[0];
	double G = mean(src_split[1])[0];
	double R = mean(src_split[2])[0];

	double KB = (R + G + B) / (3 * B);
	double KG = (R + G + B) / (3 * G);
	double KR = (R + G + B) / (3 * R);

	src_split[0] *= KB;
	src_split[1] *= KG;
	src_split[2] *= KR;

	merge(src_split, bal);

	//第三步：直方图均衡
	cvtColor(bal, bal, COLOR_BGR2HSV);
	split(bal, dst_split);
	equalizeHist(dst_split[2], dst_split[2]);
	merge(dst_split, dst);
}

void rectMat(Mat& frame) {
	rectangle(frame, Rect(Point((frame.cols - sz2.width) / 2, (frame.rows - sz2.height) / 2), sz2), Scalar(0, 255, 255));
	rectangle(frame, Rect(Point((frame.cols - sz2.width) / 2 - sz2.width, (frame.rows - sz2.height) / 2), sz2), Scalar(0, 255, 255));//左边
	rectangle(frame, Rect(Point((frame.cols + sz2.width) / 2, (frame.rows - sz2.height) / 2), sz2), Scalar(0, 255, 255));//右边
	rectangle(frame, Rect(Point((frame.cols - sz2.width) / 2, (frame.rows - sz2.height) / 2 - sz2.height), sz2), Scalar(0, 255, 255));//上边
	rectangle(frame, Rect(Point((frame.cols - sz2.width) / 2, (frame.rows + sz2.height) / 2), sz2), Scalar(0, 255, 255));//下边

	rectangle(frame, Rect(Point((frame.cols - sz2.width) / 2 - sz2.width, (frame.rows - sz2.height) / 2 - sz2.height), sz2), Scalar(0, 255, 255));//左上角
	rectangle(frame, Rect(Point((frame.cols + sz2.width) / 2, (frame.rows - sz2.height) / 2 - sz2.height), sz2), Scalar(0, 255, 255));//右上角
	rectangle(frame, Rect(Point((frame.cols - sz2.width) / 2 - sz2.width, (frame.rows + sz2.height) / 2), sz2), Scalar(0, 255, 255));//左下角
	rectangle(frame, Rect(Point((frame.cols + sz2.width) / 2, (frame.rows + sz2.height) / 2), sz2), Scalar(0, 255, 255));//右下角

}

void build_data(Mat sample_hsv, float sample_data[][3]) {
	for (int n = 0; n < 9; n++) {
		int num = 0;
		for (int j = cube[n].x; j < cube[n].x + sz.width; j++)
			for (int z = cube[n].y; z < cube[n].y + sz.height; z++) {
				sample_data[n][0] += sample_hsv.at<Vec3b>(z, j)[0];
				sample_data[n][1] += sample_hsv.at<Vec3b>(z, j)[1];
				sample_data[n][2] += sample_hsv.at<Vec3b>(z, j)[2];
				num++;
			}

		for (int j = 0; j < 3; j++)
			sample_data[n][j] /= num;
	}
}

void capture(Mat& sample_src) {
	VideoCapture cap(comeraID);
	if (!cap.isOpened()) {
		cout << "摄像头打开失败" << endl;
		exit(0);
	}
	Mat frame;
	for (;;) {
		cap >> frame;
		if (frame.empty()) break;
		rectMat(frame);
		imshow("frame", frame);
		char a = waitKey(33);
		if (a == 27) exit(0);
		else if (a == 'a') { sample_src = frame.clone(); break; }
	}
	destroyWindow("frame");
	cap.release();
	frame.release();
}

void test() {
	Mat test_src, test_hsv;
	test_src = imread("C:\\Users\\HP\\Desktop\\blue\\1.jpg");
	preparation(test_src, test_hsv);//图像预处理

	Mat goal_src, goal_hsv;
	goal_src = imread("C:\\Users\\HP\\Desktop\\green\\1.jpg");
	preparation(goal_src, goal_hsv);//图像预处理

	for (int i = 0; i < goal_hsv.cols; i++) {
		for (int j = 0; j < goal_hsv.rows; j++) {
			printf("blue:(%d,%d,%d)  green:(%d,%d,%d)\n", test_hsv.at<Vec3b>(i, j)[0], test_hsv.at<Vec3b>(i, j)[1], test_hsv.at<Vec3b>(i, j)[2], goal_hsv.at<Vec3b>(i, j)[0], goal_hsv.at<Vec3b>(i, j)[1], goal_hsv.at<Vec3b>(i, j)[2]);
		}
	}
}