#pragma once
#pragma execution_character_set("gbk")

#include<iostream> 
#include <vector>
#include <sstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <fstream>
#include <io.h>
#include <functional>
#include <time.h>
#include <numeric>
#include <algorithm>
#include <omp.h>//���߳�

#define pi 3.1416

using namespace cv;
using namespace std;

typedef pair<Point2f, float> CIRCLE;

class ScoreCircle
{
public:
	vector<CIRCLE> circles;
	vector<float> scores;
	ScoreCircle(vector<CIRCLE>c, vector<float>s);
	ScoreCircle();
};

class Parameter
{
public:
	const int kernel_size;
	const float min_radius;
	const float max_radius;
	const float area_rate;
	const bool findOverLap;
	const bool parameter_adjust;
	const bool isVisualization;
	int wait_time;
	Parameter(int kernelSize, float minRadius, float maxRadius, float areaRate, bool findOverLap, bool parameterAdjust, bool visualization, int wait_time);
};

class GrayThreshold
{
public:
	int grayThd_low;
	int grayThd_high;
	int center_low;
	int center_middle;
	int center_high;

};

//˫��˹�������С��ģ��
void minVarThd(const Mat &src_gray, int &thd_minVar, pair<int, float> &leftGauss, pair<int, float>& rightGauss, bool delZero);

//�õ������Ҷ���ֵ�ָ�ͼ��
void minVarSegmantation(const Mat &src_gray, GrayThreshold &output_gthd, const Parameter &param);

void extractContours(Mat &bw_img, vector<vector<Point>> &pCountour_all, const Parameter &param);

//�����ص�Һ��
void findOverlaps(const Mat &src_gray, const GrayThreshold &input_gthd, const Parameter param, 
										Mat & output_img, vector<CIRCLE> &overlapCircles);

//��Һ�λ���
void drawSingleDrop(const Mat &drops_without_dark, Mat &output_img, ScoreCircle &sCircle, const Parameter &param, const GrayThreshold &gthd);

//��ͨ��ָҺ��
void findSingleDrop(const Mat &src_bw_img, ScoreCircle &scoreCircles, const Parameter &param, int dev=0);

//��Һ��refine
void singleDropRefine(const Mat &src_img, CIRCLE oneCircle, CIRCLE &refineCircle);

//����Һ��
void findDroplet(Mat &drops_without_dark, Mat &mask, ScoreCircle &sCircle, Mat &bw_drop_img, const Parameter &param, const GrayThreshold &gthd);

//�ܶ���������Һ��
void findDrops(const Mat &dst_img, ScoreCircle &scoreCircles, const Parameter &param);

//NMS����Һ�ηָ���
void NMS(const ScoreCircle sCircles, ScoreCircle &outputScoreCircles, bool useArea=0);

//�ϲ����
void mergeDropResult(const Mat &src_bw_img, ScoreCircle contected_drop_circles, ScoreCircle single_drop_circles, ScoreCircle &mergedCircles, const Parameter &param);

//��������Բ�ཻ�����
float intersectionArea(CIRCLE circle_1, CIRCLE circle_2);

//����Բ����
//void circleFilter(const vector<CIRCLE> &drop_circles, vector<CIRCLE> &outputCircles);

//GMM�ָ�ͼ��
void segmentationGMM(Mat &src);

//����Һ������
void findLightDrop(const Mat &src_gray, vector<CIRCLE> &final_circles, const Parameter &param, const int dev);

//�ն����
void fillHole(const Mat src_bw, Mat &dst_bw);