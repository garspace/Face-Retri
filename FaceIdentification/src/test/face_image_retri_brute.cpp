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

using namespace seeta;


#ifdef _WIN32
std::string DATA_DIR = "../../data/";
std::string MODEL_DIR = "../../model/";
#else
std::string DATA_DIR = "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/data/";
std::string MODEL_DIR = "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/model/";
#endif

struct node {
  float score;
  std::string file_name;
};
node RES;

bool cmp(node& a, node& b){
  return a.score > b.score;
}

int main(int argc, char* argv[]) {
  // Initialize face detection model
  seeta::FaceDetection detector("/home/muyu/Documents/github/SeetaFaceEngine-master/FaceDetection/model/seeta_fd_frontal_v1.0.bin");
  detector.SetMinFaceSize(40);
  detector.SetScoreThresh(2.f);
  detector.SetImagePyramidScaleFactor(0.8f);
  detector.SetWindowStep(4, 4);

  FaceIdentification face_recognizer("/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/model/seeta_fr_v1.0.bin");
  std::string test_dir = DATA_DIR + "test_face_recognizer/";

  seeta::FaceAlignment point_detector("/home/muyu/Documents/github/SeetaFaceEngine-master/FaceAlignment/model/seeta_fa_v1.1.bin");

  //load image
  // 要加载的人脸数据
  cv::Mat gallery_img_color = cv::imread(test_dir + "images/compare_im/Aaron_Peirsol_0001.jpg", cv::IMREAD_UNCHANGED);
  cv::Mat gallery_img_gray;
  cv::cvtColor(gallery_img_color, gallery_img_gray, cv::COLOR_BGR2GRAY);
  
  ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
  gallery_img_data_color.data = gallery_img_color.data;

  ImageData gallery_img_data_gray;
  gallery_img_data_gray.data = gallery_img_gray.data;
  gallery_img_data_gray.width = gallery_img_gray.cols;
  gallery_img_data_gray.height = gallery_img_gray.rows;
  gallery_img_data_gray.num_channels = 1;

  // Detect faces
  std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
  int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

  cv::String folder = "/home/muyu/Documents/github/SeetaFaceEngine-master/FaceIdentification/data/test_face_recognizer/images/src";
  std::vector<cv::String> imagePathList;
  cv::glob(folder,imagePathList);

  // 在数据库中检测
  auto start = std::chrono::system_clock::now();
  std::vector<node> res;

  // Detect 5 facial landmarks
  seeta::FacialLandmark gallery_points[5];
  point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);
  
  float gallery_fea[2048];
  face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
  // for (int i = 0; i < 2048; i++)
  //   std::cout << gallery_fea[i] << std::endl;

  for (int i = 0; i < imagePathList.size(); i++)
  {
    cv::Mat probe_img_color = cv::imread(imagePathList[i], 1);
    cv::Mat probe_img_gray;
    cv::cvtColor(probe_img_color, probe_img_gray, cv::COLOR_BGR2GRAY);

    ImageData probe_img_data_color(probe_img_color.cols, probe_img_color.rows, probe_img_color.channels());
    probe_img_data_color.data = probe_img_color.data;

    ImageData probe_img_data_gray;
    probe_img_data_gray.data = probe_img_gray.data;
    probe_img_data_gray.width = probe_img_gray.cols;
    probe_img_data_gray.height = probe_img_gray.rows;
    probe_img_data_gray.num_channels = 1;

    std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
    int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

    if (gallery_face_num == 0 && probe_face_num==0)
    {
        std::cout << "Faces are not detected.";
        return 0;
    }

    seeta::FacialLandmark probe_points[5];
    point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);

    // Extract face identity feature
    float probe_fea[2048];
    face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);

    // Caculate similarity of two faces
    float sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);
    std::cout << "Similarity: " << sim;
    RES.score = sim;
    RES.file_name = imagePathList[i].substr(imagePathList[i].find_last_of('/')+1);
    std::cout << ", " << RES.file_name << std::endl;
    res.push_back(RES);
  }
  std::sort(res.begin(), res.end(), cmp);
  for (int i = 0; i < 5; i++) {
    std::cout << res[i].score << " " << res[i].file_name << std::endl;
  }
  auto end  = std::chrono::system_clock::now();
  std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
  printf("All the picture was cost %.2f seconds.\n", time_span);
  return 0;
}
