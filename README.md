# WSCNet: Biomedical image recognition for cell encapsulated microfluidic droplets 

[English](README.md) | [简体中文](README.zh-CN.md)

**This project is no longer under maintenance. Please visit the new version of it: [WSCNet2](https://github.com/Loyage/WSCNet2).**

# Introduction

This code is a biomedical image recognition algorithm designed for cell encapsulated microfluidic droplets, which can effectively analyze the number and size of droplets in the image, and determine the number of cells contained in each droplet, thus providing a method to automatically analyze the microdroplet wrapping experiment, and improving the analysis efficiency of the results of the microdroplet generation experiment.

Example result:

![image-20230711143425309](./imgs/result_presentation.png)

Example images and their recognition results can be found in [drop_counting/examples](https://github.com/Loyage/WSCNet/tree/master/drop_counting/examples).

# Methods of running the program

This project uses both C++ and Python languages, in which the Python part is mainly responsible for calling the trained deep learning model to realize cell count of the segmented microfluidic droplet images, and the main part of the code is written in C++ language, including the call to the Python program.

Before running this code, you need to install Anaconda/Miniconda. Here's how the Python environment is set up:

```
conda create -n torch python=3.8
conda activate torch
pip install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu117
pip install opencv-python
```

In the C++ part, this code provides the following two ways to run:

## Method 1: Run the code directly using the package

The advantage of this method is that users do not need to know much about C++ language and its environment configuration, so it is relatively convenient to run.

1. [Click here](https://github.com/Loyage/WSCNet/releases/download/v1.0.1/drop_counting-v1.0.1.zip) to download the package and unzip it.
2. Run `drop_counting_dark.exe` if your images were taken in the bright field, otherwise run  `drop_counting_bright.exe`. Enter the destination **folder address** of the image that you want to process.
3. Press enter and wait for the result.

## Method 2: Recompile the source code to run

This method requires users to install Visual Studio, have a basic understanding of C++, know how to configure OpenCV environment in VS. The advantage is that you can make certain modifications to the code at the source level, and customize some parameters (such as identifying the minimum size of the droplet), which please refer to [Parameter customization](#Parameter customization).

1.   Download the project source code.

     ```
     git clone https://github.com/Loyage/WSCNet.git
     ```

2.    Install Visual Studio (https://visualstudio.microsoft.com/vs/), with its open `drop_counting.sln`.

3.   Download the [OpenCV - 4.7.0](https://opencv.org/releases/) and installation, the installation path set to ` F:\software\OpenCV `, Otherwise, modify the related configuration items to the corresponding new address in Visual Studio.

4.   Start Execution in Visual Studio and enter the destination folder address when the program box appears.

5.   Press enter and wait for the result.

# Code structure

```c++
├─drop_counting.sln //Visual Studio Solution File
├─drop_counting
│  ├─examples //example images and recognization results
│  ├─main.cpp
│  ├─dropProcessing.cpp
│  ├─dropProcessing.h
│  ├─droplet_forward.py //Python code
│  └─drop_net-1.0.pt //trained network parameters
```

# Parameter customization

This project provides parameter customization function for different environments, the experimenters can adjust some parameters according to the specific situation, in order to achieve the best recognition effect.

The tunable parameters are placed at the beginning of the `main.cpp` file and their meanings are commented, as follows:

```c++
bool is_bright_field = 0; // 0 for dark field image，1 for bright field image
string conda_env_name = "torch"; // name of the conda environment
const float min_radius = 8;  // minimum radius of droplet identification
const float max_radius = 80;  // maximum radius of droplet identification
const int kernel_size = 3;  // kernel size for image dilation and erosion, depends on the clarity of the image
bool findOverLap = 1; // whether to detect overlapping targetsenv_name = "torch"; 
```

# Deep learning model structure and training methods

The deep learning model used in this code belongs to the CNN architecture, Model specific parameters can be found in [drop_counting/droplet_forward.py](https://github.com/Loyage/WSCNet/tree/master/drop_counting/droplet_forward.py).

We collected a large number of microfluidic droplet biomedical image data, and used the weakly supervised method for data annotation, and finally used these data for model training, and finally obtained the network model parameters used in the code. This program publishes data annotation results for all example images, which can be viewed and compared in [drop_counting/examples/manual_result](https://github.com/Loyage/WSCNet/tree/master/drop_counting/examples/manual_result).

During training, we set the matching response threshold σ, the small constant γ, and the weight ω to 0.98, 0.001 and 1, respectively. ReLU is adopted as the activation function in the whole network. The batch size is set to 1024. The learning rate is initialized with $10^{-4}$ and adjusted by the loss of the validation set.
