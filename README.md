# WSCNet：一种微液滴的显微图像识别算法

# 项目简介

该代码是为微液滴显微图像设计的一种识别算法，能够有效地分析该图像中的液滴数量、尺寸，并确定每个液滴中包含的细胞数量，从而提供了一种自动分析微液滴包裹实验的方法，提高了微液滴生成实验的结果分析效率。

运行结果示例：

![image-20230711143425309](https://github.com/Loyage/WSCNet/tree/master/imgs/circled_img.png)

示例图片可以在[drop_counting/examples](https://github.com/Loyage/WSCNet/tree/master/drop_counting/examples)文件夹中找到。



# 环境依赖

>  本项目需要安装Anaconda/Miniconda以方便运行

C++：

```
opencv == 4.7.0
```

Python：

```
pytorch
opencv-python
numpy
matplotlib
```



# 运行步骤

以下是直接使用打包好的程序运行的方法：

1. 安装Anaconda/Miniconda，创建新的conda环境，命名为`torch`；
2. 在该环境下安装pytorch，并安装其他依赖库；
3. [点击此处](https://github.com/Loyage/WSCNet/releases/download/v1.0.0/drop_counting-v1.0.0.zip)下载最新的程序包，并解压缩；
4. 运行`drop_counting.exe`文件，根据提示输入目标图片所在的文件夹路径；
5. 回车，等待程序输出结果，程序会在目标文件夹下生成`dropInformation`和`circledDropImg`两个文件夹，分别包含标注结果和根据标注结果绘制的标注图像。



以下是使用源代码重新构建程序运行的方法：

1. 安装Anaconda/Miniconda，创建新的conda环境，命名为`torch`；
2. 在该环境下安装pytorch，并安装其他依赖库；
3. 下载源码，解压缩，使用Visual Studio打开`drop_counting.sln`文件；
4. 安装`opencv`，并在Visual Studio配置好对应的项目属性；
5. 根据需要调整项目参数，参考[章节-参数自定义](# 参数自定义)，可跳过；
6. 在Visual Studio中执行代码，之后步骤同上。

![image-20230710211328933](https://github.com/Loyage/WSCNet/tree/master/imgs/label_result.png)



# 项目结构

```c++
├─drop_counting.sln //Visual Studio解决方案文件
├─drop_counting
│  ├─examples //示例图像及输出结果文件夹
│  ├─main.cpp
│  ├─dropProcessing.cpp
│  ├─dropProcessing.h
│  ├─droplet_forward.py //代码后续步骤需要调用的python文件
│  └─drop_net-1.0.pt //经训练得到的网络参数
```



# 参数自定义

本项目针对不同环境提供了参数自定义功能，实验者可以根据具体情况调整部分参数，以达到最好的识别效果。

该代码的可调参数均放在了`main.cpp`文件的开头，参数的含义均已注释。其中对实验结果有影响的参数如下：

```c++
	string env_name = "torch"; //使用的conda环境名称，可以修改为任意已安装好所有所需库的环境名称
	const int kernel_size = 2;  //如果液滴比较大可以适当调大

	const float min_radius = 8;  //最小半径，根据液滴大小进行修正
	const float max_radius = 50;  //最大半径，根据液滴大小进行修正

	const float areaRate = 0.5;  //固定面积占比，不需要调
	bool parameter_adjust = 0; //1表示显示中间结果，用于调参数；0表示不显示，用于批处理
	bool visualization = 0; //用于中间结果的可视化
	int wait_time = 1; //可视化等待时间
	bool findOverLap = 1; //是否检测重叠液滴

	bool method = 0; //0表示暗场，1表示明场
	int dev = 0; //明场下的修正参数
```

