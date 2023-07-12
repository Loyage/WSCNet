#pragma once
#pragma execution_character_set("gbk")

#include "dropProcessing.h"

void main(int argc, char* argv[])
{
	//�ɵ�����
	string env_name = "torch"; //ʹ�õ�conda��������
	const int kernel_size = 2;  //���ͺ͸�ʴʹ�õĺ˴�С�����Һ�γߴ�ϴ�����ʵ����

	const float min_radius = 8;  //��С�뾶������Һ�δ�С��������
	const float max_radius = 50;  //���뾶������Һ�δ�С��������

	const float areaRate = 0.5;  //�̶����ռ�ȣ�����Ҫ��
	bool findOverLap = 1; //�Ƿ����ص�Һ��

	bool method = 0; //0��ʾ������1��ʾ����
	int dev = 0; //�����µ���������

	//��ַ����
	string imgAddress = "";//ͼ���ļ��е�ַ
	cout << "Please input folder path:" << endl;
	cin >> imgAddress; //����ͼ���ļ��е�ַ
	if (imgAddress.back() != '\\') //ͼ���ļ��е�ַ����
	{
		imgAddress = imgAddress + "\\";
	}

	Parameter param(kernel_size, min_radius, max_radius, areaRate, findOverLap, parameter_adjust, visualization, wait_time);

	string imgSaveFileName= "circledDropImg"; //ͼ���������ַ
	string dropInformationAddress = "dropInformation";//Һ����Ϣ�����ַ

	string command("mkdir "+imgAddress + imgSaveFileName);
	string command2("mkdir " + imgAddress + dropInformationAddress);

	system(command.c_str());
	system(command2.c_str());
	
	string img_extend ="*.bmp"; //ͼ���׺��

	//�������в���
	vector<string> img_names;

	struct _finddata_t fileinfo;
	long long handle;
	handle = _findfirst((imgAddress + img_extend).data(), &fileinfo);
	if (handle == -1)
	{
		cout << "\n fail to find images! \n" << endl;
		system("pause");
		return;
	}
	else
	{
		cout << fileinfo.name << endl;
		img_names.push_back(fileinfo.name);
	}
	while (!_findnext(handle, &fileinfo))
	{
		cout << fileinfo.name << endl;
		img_names.push_back(fileinfo.name);
	}
	_findclose(handle);
	//system("pause");

	vector<float> spend_time;

	bool parameter_adjust = 0; //�Ƿ���ʾ�м��������ڵ�������
	bool visualization = 0; //�Ƿ���ӻ��м���
	int wait_time = 1; //���ӻ��ȴ�ʱ��

	//��¼ʱ��
	clock_t start_all, finish_all;
	float duration_all;
	start_all = clock();

#pragma omp parallel for //num_threads(6) //�߳��������޸ģ�1~6
	for (int index = 0; index < img_names.size(); index++)
	{
		string img_add = imgAddress + img_names[index];
		Mat src_color = imread(img_add);
		//resize(src_color, src_color, src_color.size());
		string label_add = imgAddress + dropInformationAddress + "\\" + img_names[index] + ".txt";
		ofstream fout(label_add);
		
		//��¼ʱ��
		clock_t start, finish;
		float duration;
		start = clock();

		//imshow("src_img", src_color);
		//waitKey();

		Mat src_gray;//��ɫͼ��ת���ɻҶ�ͼ

		cvtColor(src_color, src_gray, COLOR_RGB2GRAY);
		ScoreCircle final_circles;

		if (!method)
		{
			//1. �ָ�õ�������ֵ����������
			cout << "Step 1: Running  image segmantation by minimizing weighted sum of variances ..." << endl;
			GrayThreshold gthd;
			minVarSegmantation(src_gray, gthd, param);
			cout << "Step 1 is done! " << endl;

			//2. �޳�ͼ���е��ص�Һ��
			cout << "Step 2: Searching overlapping droplets..." << endl;
			Mat drops_without_dark;
			vector<CIRCLE> overlapCircles;
			findOverlaps(src_gray, gthd, param, drops_without_dark, overlapCircles);
			cout << "Step 2 is done!" << endl;

			//3. ������ͨ��ָ�Һ�Σ���������һ��������©��
			cout << "Step 3: Separating valid droplet-area into single droplet by analyzing connected components..." << endl;
			ScoreCircle single_drop_circles;
			Mat single_drop_img;
			drawSingleDrop(drops_without_dark, single_drop_img, single_drop_circles, param, gthd);
			cout << "Step 3 is done!" << endl;

			//4. �����ܶ������㷨�ָ�Һ��
			cout << "Step 4: Separating valid droplet-area into single droplet by exploiting density shrink algorithm..." << endl;
			ScoreCircle connected_drop_circles;
			Mat bw_drop_img;
			findDroplet(drops_without_dark, single_drop_img, connected_drop_circles, bw_drop_img, param, gthd);
			cout << "Step 4 is done!" << endl;

			//5. �ϲ�3��4�Ľ�����Ż����
			cout << "Step 5: Merging the results between step 3 and step 4..." << endl;

			mergeDropResult(bw_drop_img, connected_drop_circles, single_drop_circles, final_circles, param);
			cout << "Step 5 is done!" << endl;

		}
		else
		{
			findLightDrop(src_gray, final_circles.circles, param, dev);
		}

		Mat final_result;
		src_color.copyTo(final_result);
		for (int i = 0; i != final_circles.circles.size(); i++)
		{
			circle(final_result, final_circles.circles[i].first, final_circles.circles[i].second, Scalar(0, 0, 255), 1.5);

			Rect rect((int)(final_circles.circles[i].first.x - final_circles.circles[i].second + 1),
				(int)(final_circles.circles[i].first.y - final_circles.circles[i].second + 1),
				(int)(final_circles.circles[i].second * 2 + 0.5), (int)(final_circles.circles[i].second * 2 + 0.5));
			rect = rect&Rect(0, 0, src_gray.cols - 1, src_gray.rows - 1);

			//rectangle(final_result, rect, Scalar(255, 0, 0));

			//��������txt
			fout << final_circles.circles[i].first.x << "\t" << final_circles.circles[i].first.y << "\t" << final_circles.circles[i].second << "\t" << -1 << endl;

		}
		fout.close();


		//python sdk classification
		cout << "Droplet Classification (taking a while)..." << endl;

		// ����python���л���������python����
		const char* command = nullptr;
		string str_cmd = "activate " + env_name + "&&python droplet_forward.py " + imgAddress + " " + img_names[index];
		command = str_cmd.c_str();
		system(command);

		//spend_time.push_back(duration);

		//��ȡ�������������ӻ�����ͼ��
		ifstream infile;
		//infile.open(imgAddress + dropInformationAddress + "\\" + img_names[index] + "_circle.txt", ios::in);
		infile.open(imgAddress + dropInformationAddress + "\\" + img_names[index] + ".txt.txt", ios::in);
		if (!infile.is_open())
		{
			cout << "open file failure!" << endl;
			continue;
		}
		CIRCLE onecir;
		int cate;
		float scorecir;
		while (!infile.eof())
		{
			infile >> onecir.first.x >> onecir.first.y >> onecir.second >> cate;
			Scalar color;
			if (cate <= 0)
				color = Scalar(0, 0, 255);
			else if (cate == 1)
				color = Scalar(255, 0, 0);
			else if (cate == 2)
				color = Scalar(0, 255, 0);
			if (infile.good())
				circle(final_result, onecir.first, onecir.second, color, 1.5);
		}
		infile.close();


		cout << "The number of drops in " + img_names[index] + " is: " << final_circles.circles.size() + 1 << endl;
		imwrite(imgAddress + imgSaveFileName + "\\circled_" + img_names[index], final_result);

		if (visualization)
		{
			imshow("Final Result", final_result);
			//waitKey();
			waitKey();
			destroyAllWindows();
			system("pause");
		}

	}

	//��ʾƽ������ʱ��
	finish_all = clock();
	duration_all = (float)(finish_all - start_all) / CLOCKS_PER_SEC;

	cout << endl;
	cout << "Average Processing time per image is: " << duration_all / img_names.size() << endl;
	cout << endl;

	system("pause");

}
