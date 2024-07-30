#include <Windows.h>
#include <PComm.h>
#include "work.h"
#include "solver.h"
#include "kocicube.h"
#include "facecube.h"
#include "cubepars.h"

Ptr<SVM> red_orange = SVM::create();
Ptr<SVM> red_yellow = SVM::create();
Ptr<SVM> red_white = SVM::create();
Ptr<SVM> red_green = SVM::create();
Ptr<SVM> red_blue = SVM::create();
Ptr<SVM> orange_yellow = SVM::create();
Ptr<SVM> orange_white = SVM::create();
Ptr<SVM> orange_green = SVM::create();
Ptr<SVM> orange_blue = SVM::create();
Ptr<SVM> yellow_white = SVM::create();
Ptr<SVM> yellow_green = SVM::create();
Ptr<SVM> yellow_blue = SVM::create();
Ptr<SVM> white_green = SVM::create();
Ptr<SVM> white_blue = SVM::create();
Ptr<SVM> green_blue = SVM::create();
char solution[40];
int solutionLength;

void init();
char predict(float sample_data[]);
void F_capture(char F[]);
void B_capture(char B[]);
void U_capture(char U[]);
void D_capture(char D[]);
void R_capture(char R[]);
void L_capture(char L[]);

int main() {
	//test();
	
	char F[12] = "F:", B[12] = "B:", U[12] = "U:", D[12] = "D:", R[12] = "R:", L[12] = "L:";
	char recvBuf[40];
	unsigned int status;
	int sendLen=0, recLen=0;
	FaceletCube faceletCube;
	CubeParser cubeParser;
	
	init();//初始化支持向量机
	F_capture(F);
	B_capture(B);
	U_capture(U);
	D_capture(D);
	R_capture(R);
	L_capture(L);

	char* cu[7] = { (char*)"", (char*)F, (char*)B, (char*)U, (char*)D, (char*)R, (char*)L };
	for (int i = 1; i < 7; i++)
		cout << cu[i] << endl;
	if ((status = cubeParser.ParseInput(7, cu, faceletCube)) != CubeParser::VALID)
	{
		std::cout << cubeParser.ErrorText(status) << std::endl;
		exit(1);
	}

	KociembaCube cube;
	if ((status = faceletCube.Validate(cube)) != FaceletCube::VALID)
	{
		std::cout << faceletCube.ErrorText(status) << std::endl;
		exit(1);
	}

	Solver solver;
	solver.InitializeTables();
	(void)solver.Solve(cube, solutionLength, solution);

	sendLen = sio_write(commID, solution, solutionLength);
	printf("%d char has been send.\n", sendLen);

	while (1) {
		memset(recvBuf, 0, sizeof(recvBuf));
		recLen = sio_read(commID, recvBuf, 40);
		if (recLen != 0) {
			for (int i = 0; i < recLen; i++)
				cout << recvBuf[i] << " ";
			cout << endl;
			printf("Total:%d\n", recLen);
			break;
		}
	}
	waitKey(0);
	
	return 0;
}

void init() {
	red_orange = SVM::load("red_orange.xml");
	red_yellow = SVM::load("red_yellow.xml");
	red_white = SVM::load("red_white.xml");
	red_green = SVM::load("red_green.xml");
	red_blue = SVM::load("red_blue.xml");
	orange_yellow = SVM::load("orange_yellow.xml");
	orange_white = SVM::load("orange_white.xml");
	orange_green = SVM::load("orange_green.xml");
	orange_blue = SVM::load("orange_blue.xml");
	yellow_white = SVM::load("yellow_white.xml");
	yellow_green = SVM::load("yellow_green.xml");
	yellow_blue = SVM::load("yellow_blue.xml");
	white_green = SVM::load("white_green.xml");
	white_blue = SVM::load("white_blue.xml");
	green_blue = SVM::load("green_blue.xml");

	if (sio_open(commID) != SIO_OK) {
		printf("Com is open fail!\n");
		return;
	}
	if (sio_ioctl(commID, B115200, BIT_8 | STOP_1 | P_NONE) != SIO_OK) {
		printf("Init fail!\n");
		return;
	}
}

char predict(float sample_data[]) {
	int red = 0, orange = 0, yellow = 0, white = 0, green = 0, blue = 0;
	Mat sample_mat(1, 3, CV_32FC1, sample_data);
	if (red_orange->predict(sample_mat) == 1) red++;
	else orange++;
	if (red_yellow->predict(sample_mat) == 1) red++;
	else yellow++;
	if (red_white->predict(sample_mat) == 1) red++;
	else white++;
	if (red_green->predict(sample_mat) == 1) red++;
	else green++;
	if (red_blue->predict(sample_mat) == 1) red++;
	else blue++;

	if (orange_yellow->predict(sample_mat) == 1) orange++;
	else yellow++;
	if (orange_white->predict(sample_mat) == 1) orange++;
	else white++;
	if (orange_green->predict(sample_mat) == 1) orange++;
	else green++;
	if (orange_blue->predict(sample_mat) == 1) orange++;
	else blue++;

	if (yellow_white->predict(sample_mat) == 1) yellow++;
	else white++;
	if (yellow_green->predict(sample_mat) == 1) yellow++;
	else green++;
	if (yellow_blue->predict(sample_mat) == 1) yellow++;
	else blue++;

	if (white_green->predict(sample_mat) == 1) white++;
	else green++;
	if (white_blue->predict(sample_mat) == 1) white++;
	else blue++;

	if (green_blue->predict(sample_mat) == 1) green++;
	else blue++;

	return MaxColor(red, orange, yellow, white, green, blue);
}

void F_capture(char F[]) {
	Mat sample_src, sample_hsv;
	float sample_data[9][3];
	memset(sample_data, 0, sizeof(sample_data));

	cout << "请拍正面" << endl;
	capture(sample_src);//获取原图像
	preparation(sample_src, sample_hsv);//图像预处理
	build_data(sample_hsv, sample_data);//建立预测数据

	for (int i = 0; i < 9; i++) {
		char p = predict(sample_data[i]);
		if (p == 'O')
			if (sample_data[i][0] > 100) p = 'R';
		if (p == 'B')
			if (sample_data[i][0] > 120) p = 'G';
		F[i + 2] = p;
	}
	F[11] = '\0';
}

void B_capture(char B[]) {
	Mat sample_src, sample_hsv;
	float sample_data[9][3];
	memset(sample_data, 0, sizeof(sample_data));

	cout << "请拍背面" << endl;
	capture(sample_src);//获取原图像
	preparation(sample_src, sample_hsv);//图像预处理
	build_data(sample_hsv, sample_data);//建立预测数据

	for (int i = 0; i < 9; i++) {
		char p = predict(sample_data[i]);
		if (p == 'O')
			if (sample_data[i][0] > 100) p = 'R';
		if (p == 'B')
			if (sample_data[i][0] > 120) p = 'G';
		B[i + 2] = p;
	}
	B[11] = '\0';
}

void U_capture(char U[]) {
	Mat sample_src, sample_hsv;
	float sample_data[9][3];
	memset(sample_data, 0, sizeof(sample_data));

	cout << "请拍上面" << endl;
	capture(sample_src);//获取原图像
	preparation(sample_src, sample_hsv);//图像预处理
	build_data(sample_hsv, sample_data);//建立预测数据

	for (int i = 0; i < 9; i++) {
		char p = predict(sample_data[i]);
		if (p == 'O')
			if (sample_data[i][0] > 100) p = 'R';
		if (p == 'B')
			if (sample_data[i][0] > 120) p = 'G';
		U[i + 2] = p;
	}
	U[11] = '\0';
}

void D_capture(char D[]) {
	Mat sample_src, sample_hsv;
	float sample_data[9][3];
	memset(sample_data, 0, sizeof(sample_data));

	cout << "请拍下面" << endl;
	capture(sample_src);//获取原图像
	preparation(sample_src, sample_hsv);//图像预处理
	build_data(sample_hsv, sample_data);//建立预测数据

	for (int i = 0; i < 9; i++) {
		char p = predict(sample_data[i]);
		if (p == 'O')
			if (sample_data[i][0] > 100) p = 'R';
		if (p == 'B')
			if (sample_data[i][0] > 120) p = 'G';
		D[i + 2] = p;
	}
	D[11] = '\0';
}

void R_capture(char R[]) {
	Mat sample_src, sample_hsv;
	float sample_data[9][3];
	memset(sample_data, 0, sizeof(sample_data));

	cout << "请拍右面" << endl;
	capture(sample_src);//获取原图像
	preparation(sample_src, sample_hsv);//图像预处理
	build_data(sample_hsv, sample_data);//建立预测数据

	for (int i = 0; i < 9; i++) {
		char p = predict(sample_data[i]);
		if (p == 'O')
			if (sample_data[i][0] > 100) p = 'R';
		if (p == 'B')
			if (sample_data[i][0] > 120) p = 'G';
		R[i + 2] = p;
	}
	R[11] = '\0';
}

void L_capture(char L[]) {
	Mat sample_src, sample_hsv;
	float sample_data[9][3];
	memset(sample_data, 0, sizeof(sample_data));

	cout << "请拍左面" << endl;
	capture(sample_src);//获取原图像
	preparation(sample_src, sample_hsv);//图像预处理
	build_data(sample_hsv, sample_data);//建立预测数据

	for (int i = 0; i < 9; i++) {
		char p = predict(sample_data[i]);
		if (p == 'O')
			if (sample_data[i][0] > 100) p = 'R';
		if (p == 'B')
			if (sample_data[i][0] > 120) p = 'G';
		L[i + 2] = p;
	}
	L[11] = '\0';
}