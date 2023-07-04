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
#include <omp.h>//多线程

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

//双高斯方差和最小化模型
void minVarThd(const Mat &src_gray, int &thd_minVar, pair<int, float> &leftGauss, pair<int, float>& rightGauss, bool delZero);

//得到两个灰度阈值分割图像
void minVarSegmantation(const Mat &src_gray, GrayThreshold &output_gthd, const Parameter &param);

void extractContours(Mat &bw_img, vector<vector<Point>> &pCountour_all, const Parameter &param);

//搜索重叠液滴
void findOverlaps(const Mat &src_gray, const GrayThreshold &input_gthd, const Parameter param, 
										Mat & output_img, vector<CIRCLE> &overlapCircles);

//单液滴绘制
void drawSingleDrop(const Mat &drops_without_dark, Mat &output_img, ScoreCircle &sCircle, const Parameter &param, const GrayThreshold &gthd);

//连通域分割单液滴
void findSingleDrop(const Mat &src_bw_img, ScoreCircle &scoreCircles, const Parameter &param, int dev=0);

//单液滴refine
void singleDropRefine(const Mat &src_img, CIRCLE oneCircle, CIRCLE &refineCircle);

//搜索液滴
void findDroplet(Mat &drops_without_dark, Mat &mask, ScoreCircle &sCircle, Mat &bw_drop_img, const Parameter &param, const GrayThreshold &gthd);

//密度收缩搜索液滴
void findDrops(const Mat &dst_img, ScoreCircle &scoreCircles, const Parameter &param);

//NMS过滤液滴分割结果
void NMS(const ScoreCircle sCircles, ScoreCircle &outputScoreCircles, bool useArea=0);

//合并结果
void mergeDropResult(const Mat &src_bw_img, ScoreCircle contected_drop_circles, ScoreCircle single_drop_circles, ScoreCircle &mergedCircles, const Parameter &param);

//计算两个圆相交的面积
float intersectionArea(CIRCLE circle_1, CIRCLE circle_2);

//交叠圆过滤
//void circleFilter(const vector<CIRCLE> &drop_circles, vector<CIRCLE> &outputCircles);

//GMM分割图像
void segmentationGMM(Mat &src);

//明场液滴搜索
void findLightDrop(const Mat &src_gray, vector<CIRCLE> &final_circles, const Parameter &param, const int dev);

//空洞填充
void fillHole(const Mat src_bw, Mat &dst_bw);