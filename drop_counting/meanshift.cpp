#pragma once
#pragma execution_character_set("gbk")

/*缩放图形搜索算法*/
/*	0. 如果区域有效面积与图形面积大小相等，则退出，输出当前图形。
1. 如果有效面积小于图形面积，则利用有效面积缩放图形，然后固定图形大小，利用meanshift找到最密集区域（最接近图形的区域），
2. 如果该区域有效面积与图形面积相等，则结束搜索，找到roi中最大图形
3. 如果该区域有效面积小于图形面积，则说明当前roi中没有与缩放图形大小相同的图形，则回到1
*/
/****meanshift+半径收缩算法，依赖于初始搜索位置和初始搜索半径，容易陷入局部极小值****/

//vector<pair<Point2f, float>> result_circles;//存圆
//vector<pair<float,int>> result_scores;//存圆面积
//int circle_index(0);
//vector<Point2f> nine_centers; //roi分为9个不重叠的区域开始迭代，防止对称引起的极小值
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6, roi_rect.y + (float)roi_rect.height / 6));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6 * 3, roi_rect.y + (float)roi_rect.height / 6));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6 * 5, roi_rect.y + (float)roi_rect.height / 6));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6, roi_rect.y + (float)roi_rect.height / 6 * 3));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6 * 3, roi_rect.y + (float)roi_rect.height / 6 * 3));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6 * 5, roi_rect.y + (float)roi_rect.height / 6 * 3));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6, roi_rect.y + (float)roi_rect.height / 6 * 5));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6 * 3, roi_rect.y + (float)roi_rect.height / 6 * 5));
//nine_centers.push_back(Point2f(roi_rect.x + (float)roi_rect.width / 6 * 5, roi_rect.y + (float)roi_rect.height / 6 * 5));
//for (vector<Point2f>::iterator iter = nine_centers.begin(); iter != nine_centers.end(); iter++)
//{

//	//vector<Point2f> nine_centers;
//	Point2f final_center(0, 0);
//	//vector<float> nine_radiuses;
//	float final_radius(0);

//	destroyAllWindows();
//	if (isDisplay)
//	{
//		namedWindow("mask_dst");
//		moveWindow("mask_dst", 3000, 0);
//		imshow("mask_dst", mask_dst);
//		waitKey();
//	}

//	//内接圆内全白的正方形mask
//	float old_radius = start_radius;

//	Point2f old_center = *iter;

//	zeros_img = Mat::zeros(dst_img.size(), dst_img.type());// 全0图
//	circle(zeros_img, old_center, old_radius, 255, -1, CV_8U);
//	Mat roi_circle(zeros_img, roi_rect);

//	Mat circle_dst;  //circle mask下的轮廓内部区域
//	multiply(mask_dst, roi_circle, circle_dst);

//	/*display*/
//	if (isDisplay)
//	{
//		Mat display_circle = Mat::zeros(dst_img.size(), dst_img.type());// 全0图
//		cvtColor(display_circle, display_circle, CV_GRAY2BGR);
//		circle(display_circle, old_center, old_radius, Scalar(0, 0, 255), 1);
//		circle(display_circle, old_center, 2, Scalar(0, 0, 255), -1);
//		cout << display_circle.channels();
//		Mat roi_display(display_circle, roi_rect);
//		Mat display_img;
//		circle_dst.copyTo(display_img);
//		cvtColor(display_img, display_img, CV_GRAY2BGR);
//		add(roi_display, display_img, display_img);
//		namedWindow("circle_dst");
//		moveWindow("circle_dst", 3200, 0);
//		imshow("circle_dst", display_img);
//		waitKey();
//	}
//	/***********/

//	float circling_countour_area = countNonZero(circle_dst); //当前非零数
//	float new_radius = sqrt(circling_countour_area / 3.1416);

//	int radius_count = 0;
//	bool flag = 0;
//	while (((new_radius*new_radius) / (old_radius*old_radius)) < 0.95) //有效面积与图形面积之比
//	{

//		radius_count++;
//		old_radius = new_radius;
//		//执行一步图形缩放
//		zeros_img = Mat::zeros(dst_img.size(), dst_img.type());// 全0图
//		circle(zeros_img, old_center, old_radius, 255, -1, CV_8U);
//		Mat roi_circle(zeros_img, roi_rect);

//		multiply(mask_dst, roi_circle, circle_dst);//circle mask下的轮廓内部区域

//		/***display***/
//		if (isDisplay)
//		{
//			Mat display_circle = Mat::zeros(dst_img.size(), dst_img.type());// 全0图
//			cvtColor(display_circle, display_circle, CV_GRAY2BGR);
//			circle(display_circle, old_center, old_radius, Scalar(0, 0, 255), 1);
//			circle(display_circle, old_center, 2, Scalar(0, 0, 255), -1);
//			cout << display_circle.channels();
//			Mat roi_display(display_circle, roi_rect);
//			Mat display_img;
//			circle_dst.copyTo(display_img);
//			cvtColor(display_img, display_img, CV_GRAY2BGR);
//			add(roi_display, display_img, display_img);
//			string radius_name = "circle_dst_for_radius" + to_string(radius_count);
//			namedWindow(radius_name);
//			moveWindow(radius_name, 3200, radius_count*window_step);
//			cout << display_img.channels();
//			imshow(radius_name, display_img);
//			waitKey();
//		}
//		/**************/

//		//计算区域重心
//		Moments moment = moments(circle_dst, false);
//		float mass_x(0), mass_y(0);
//		if (moment.m00 != 0)//除数不能为0
//		{
//			mass_x = cvRound(moment.m10 / moment.m00);//计算重心横坐标
//			mass_y = cvRound(moment.m01 / moment.m00);//计算重心纵坐标
//		}
//		Point2f new_center;
//		new_center.x = mass_x + roi_rect.x;
//		new_center.y = mass_y + roi_rect.y;
//		/*meanshift算法搜索最佳圆心center（固定半径搜索最大密度区域）*/
//		int center_count = 0;
//		while (sqrt((old_center.x - new_center.x)*(old_center.x - new_center.x) + (old_center.y - new_center.y)*(old_center.y - new_center.y)) > 0.1)
//		{
//			center_count++;
//			old_center = new_center;
//			zeros_img = Mat::zeros(dst_img.size(), dst_img.type());// 全0图
//			circle(zeros_img, old_center, new_radius, 255, -1, CV_8U);
//			Mat roi_circle(zeros_img, roi_rect);

//			multiply(mask_dst, roi_circle, circle_dst);//circle mask下的轮廓内部区域

//			/***display***/
//			if (isDisplay)
//			{
//				Mat display_circle = Mat::zeros(dst_img.size(), dst_img.type());// 全0图
//				cvtColor(display_circle, display_circle, CV_GRAY2BGR);
//				circle(display_circle, old_center, old_radius, Scalar(0, 0, 255), 1);
//				circle(display_circle, old_center, 2, Scalar(0, 0, 255), -1);
//				cout << display_circle.channels();
//				Mat roi_display(display_circle, roi_rect);
//				Mat display_img;
//				circle_dst.copyTo(display_img);
//				cvtColor(display_img, display_img, CV_GRAY2BGR);
//				add(roi_display, display_img, display_img);
//				string center_name = "circle_dst_for_center" + to_string(radius_count) + to_string(center_count);
//				namedWindow(center_name);
//				moveWindow(center_name, 3200 + center_count * window_step, radius_count * window_step);
//				imshow(center_name, display_img);
//				waitKey();
//			}
//			/**************/


//			Moments moment = moments(circle_dst, false);
//			float mass_x(0), mass_y(0);
//			if (moment.m00 != 0)//除数不能为0
//			{
//				mass_x = cvRound(moment.m10 / moment.m00);//计算重心横坐标
//				mass_y = cvRound(moment.m01 / moment.m00);//计算重心纵坐标
//			}
//			new_center.x = mass_x + roi_rect.x;
//			new_center.y = mass_y + roi_rect.y;

//		}
//		final_center = new_center;

//		float new_circling_countour_area = countNonZero(circle_dst); //当前非零数
//		new_radius = sqrt(new_circling_countour_area / 3.1416) + 1; //margin为1

//		if (new_circling_countour_area < min_area)//如果圆缩小至min_area后break出去
//		{
//			flag = 1;
//			break;
//		}
//	}

//	if (flag)
//		continue;

//	final_radius = new_radius;
//	//nine_centers.push_back(final_center);
//	//nine_radiuses.push_back(final_radius);
//	float final_area = 3.1416*final_radius*final_radius;
//	if (final_area > min_area)
//	{
//		result_circles.push_back(pair<Point2f, float>(final_center, final_radius));
//		result_scores.push_back(pair<float,int>(final_area, circle_index));
//		circle_index++;
//	}

//}

//if (!result_scores.size())
//	return;

////面积排序得到从大到小的score
//std::sort(result_scores.begin(), result_scores.end(),greater<pair<float,int>>());

///*非极大值抑制NMS
//以排序后的圆面积作为score，以两个圆iou作为iou
//*/
//vector<pair<Point2f, float>> nms_circles; //存nms后的circles
//vector<int> delete_index;
//for (int i_ter = 0; i_ter != result_scores.size(); i_ter++)
//{
//	vector<int>::iterator id_iter = find(delete_index.begin(), delete_index.end(), result_scores[i_ter].second);
//	if (id_iter != delete_index.end())
//		continue;
//	pair<Point2f, float> circle_1 = result_circles[result_scores[i_ter].second];
//	nms_circles.push_back(result_circles[result_scores[i_ter].second]);
//	for (int j_ter = i_ter + 1; j_ter != result_scores.size(); j_ter++)
//	{
//		pair<Point2f, float> circle_2 = result_circles[result_scores[j_ter].second];
//		float areaInt = intersectionArea(circle_1, circle_2);
//		float area_1 = pi*circle_1.second*circle_1.second;
//		float area_2 = pi*circle_2.second*circle_2.second;
//		float areaUnion = area_1 + area_2 - areaInt;
//		float IOU = areaInt / areaUnion;
//		if (IOU > 0.5 || areaInt / area_1 > 0.5 || areaInt / area_2 > 0.5)
//			delete_index.push_back(result_scores[j_ter].second);
//	}
//}

//for (int k = 0; k != nms_circles.size(); k++)
//{
//	circle(drops_img, nms_circles[k].first, nms_circles[k].second+3, Scalar(255, 0, 0), 1);
//	//imshow("drops_img", drops_img);
//	//waitKey();
//}