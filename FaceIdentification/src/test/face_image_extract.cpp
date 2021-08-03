/*
*
* This file is part of the open-source SeetaFace engine, which includes three modules:
* SeetaFace Detection, SeetaFace Alignment, and SeetaFace Identification.
*
* This file is part of the SeetaFace Identification module, containing codes implementing the
* face identification method described in the following paper:
*
*j
*   VIPLFaceNet: An Open Source Deep Face Recognition SDK,
*   Xin Liu, Meina Kan, Wanglong Wu, Shiguang Shan, Xilin Chen.
*   In Frontiers of Computer Science.
*
*
* Copyright (C) 2016, Visual Information Processing and Learning (VIPL) group,
* Institute of Computing Technology, Chinese Academy of Sciences, Beijing, China.
*
* The codes are mainly developed by Jie Zhang(a Ph.D supervised by Prof. Shiguang Shan)
*
* As an open-source face recognition engine: you can redistribute SeetaFace source codes
* and/or modify it under the terms of the BSD 2-Clause License.
*
* You should have received a copy of the BSD 2-Clause License along with the software.
* If not, see < https://opensource.org/licenses/BSD-2-Clause>.
*
* Contact Info: you can send an email to SeetaFace@vipl.ict.ac.cn for any problems.
*
* Note: the above information must be kept whenever or wherever the codes are used.
*
*/

#include<iostream>
using namespace std;

#ifdef _WIN32
#pragma once
#include <opencv2/core/version.hpp>

#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) \
  CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif //_DEBUG

#pragma comment( lib, cvLIB("core") )
#pragma comment( lib, cvLIB("imgproc") )
#pragma comment( lib, cvLIB("highgui") )

#endif //_WIN32

#if defined(__unix__) || defined(__APPLE__)

#ifndef fopen_s

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

#endif //fopen_s

#endif //__unix

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"

#include "face_identification.h"
#include "recognizer.h"
#include "face_detection.h"
#include "face_alignment.h"

#include "math_functions.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <fstream>

using namespace seeta;

#ifdef _WIN32
std::string DATA_DIR = "../../data/";
std::string MODEL_DIR = "../../model/";
#else
std::string DATA_DIR = "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/data/";
std::string MODEL_DIR = "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/model/";
std::string ROOT = "/home/muyu/Documents/github/SeetaFaceEngine-master/";
#endif

void save_data(ofstream& file, std::vector<cv::String>& imagePathList, 
              seeta::FaceDetection& detector, seeta::FaceAlignment& point_detector,
              FaceIdentification& face_recognizer) {
  cv::Mat probe_img_color;
  cv::Mat probe_img_gray;
  ImageData probe_img_data_gray;
  ImageData probe_img_data_color;
  std::vector<seeta::FaceInfo> probe_faces;
  int32_t probe_face_num;
  seeta::FacialLandmark probe_points[5];
  float probe_fea[2048];

  for (int i = 0; i < imagePathList.size(); i++) {
    probe_img_color = cv::imread(imagePathList[i], 1);
    cv::cvtColor(probe_img_color, probe_img_gray, cv::COLOR_BGR2GRAY);

    probe_img_data_color.data = probe_img_color.data;
    probe_img_data_color.width = probe_img_color.cols;
    probe_img_data_color.height = probe_img_color.rows;
    probe_img_data_color.num_channels = 3;

    probe_img_data_gray.data = probe_img_gray.data;
    probe_img_data_gray.width = probe_img_gray.cols;
    probe_img_data_gray.height = probe_img_gray.rows;
    probe_img_data_gray.num_channels = 1;

    probe_faces = detector.Detect(probe_img_data_gray);
    probe_face_num = static_cast<int32_t>(probe_faces.size());

    if (probe_face_num == 0) {
        std::cout << "Faces are not detected.";
        std::fill(probe_fea, probe_fea+2048, 0);
    }
    else {
      point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);
      // Extract face identity feature
      face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);
    }

    for (int i = 0; i < 2048; i++) {
      if (i == 0)
        file << probe_fea[i] ;
      else
        file << "\t" << probe_fea[i];
    }
    file << std::endl;
  }
  file.close();
}

int main(int argc, char* argv[]) {
  // 参数检查 ================================================================
  // 第一个参数表示当前处理数据集还是查询数据
  // 第二个参数表示图像的路径
  // 第三个参数表示提取特征的存储路径
  if (argc != 4) {
    std::cout << "Please input path of query data and data base." << std::endl;
    return 0;
  }

  // 如果是 0，提取查询数据特征并存储
  // 如果是 1，提取数据集特征并存储
  int isQuery = std::stoi(argv[1]);

  // 读取图像的路径
  cv::String folder = argv[2];
  std::string save_path = argv[3];

  // cmake ..; make; ./src/test/face_image_extract.bin 0 /home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/data/test_face_recognizer/images/query /home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/build/

  // ./src/test/face_image_extract.bin 0 /home/muyu/Downloads/query_path /home/muyu/Documents/github/LSH/data/query

  // 模型加载 ================================================================
  // 人脸检测的模型
  seeta::FaceDetection detector("/home/muyu/Documents/github/SeetaFaceEngine-master/FaceDetection/model/seeta_fd_frontal_v1.0.bin");
  detector.SetMinFaceSize(40);
  detector.SetScoreThresh(2.f);
  detector.SetImagePyramidScaleFactor(0.8f);
  detector.SetWindowStep(4, 4);

  // 人脸检测
  FaceIdentification face_recognizer("/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/model/seeta_fr_v1.0.bin");

  // 关键点检测
  seeta::FaceAlignment point_detector("/home/muyu/Documents/github/SeetaFaceEngine-master/FaceAlignment/model/seeta_fa_v1.1.bin");
  // ==========================================================================

  // 录入数据 =======================================================
  ofstream file_;
  file_.open(save_path);
  
  std::vector<cv::String> imagePathList;

  cv::glob(folder, imagePathList);
  save_data(file_, imagePathList, detector, point_detector, face_recognizer);
  
  return 0;
}
