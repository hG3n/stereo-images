#include <iostream>
#include <vector>
#include <string>
#include <omp.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

void loadImages(std::string folder, std::string filetype ,int number, std::vector<cv::Mat> *store) {
  for(int i = 1; i < number+1; ++i){
    std::string num = std::to_string(i);
    cv::Mat img;
    if(i < 10) 
      img = cv::imread(folder+"00"+num+filetype);
    else
      img = cv::imread(folder+"0"+num+filetype);
    store->push_back(img);
    img.release();
  }
}

void saveCoefficients(std::string filename,
                      cv::Mat matrix,
                      cv::Mat distCoeffs,
                      cv::Mat R,
                      cv::Mat T,
                      cv::Mat E,
                      cv::Mat F) {

  cv::FileStorage fs(filename, cv::FileStorage::WRITE);
  fs << "cameraMatrix" << matrix;
  fs << "distCoeff" << distCoeffs;
  fs << "rotationMatrix" << R;
  fs << "translationMatrix" << T;
  fs << "essentialMatrix" << E;
  fs << "fundamentalMatrix" << F;
  fs.release();
}

int main() {

  // settings
  int hCorners = 9;
  int vCorners = 6;

  int numSquares = hCorners * vCorners;
  cv::Size boardSize = cv::Size(hCorners, vCorners);

  // calibration storages
  std::vector<cv::Mat> leftImages;
  std::vector<cv::Mat> rightImages;

  // define the empty calibration matrix
  cv::Mat intrinsicLeft = cv::Mat(3, 3, CV_32FC1);
  intrinsicLeft.ptr<float>(0)[0] = 1;
  intrinsicLeft.ptr<float>(1)[1] = 1;

  cv::Mat intrinsicRight = cv::Mat(3, 3, CV_32FC1);
  intrinsicRight.ptr<float>(0)[0] = 1;
  intrinsicRight.ptr<float>(1)[1] = 1;

  cv::Mat R; // rotation Matrix between left and right image coordinate systems
  cv::Mat T; // translation vector between left and right coord system
  cv::Mat E; // essential Matrix
  cv::Mat F; // fundamental Matrix

  // more calibration vars
  cv::Mat distCoeffsLeft;
  cv::Mat distCoeffsRight;

  // containers for obj and img points
  std::vector<std::vector<cv::Point3f>> objectPoints;
  std::vector<cv::Point3f> obj;

  std::vector<std::vector<cv::Point2f>> imagePointsLeft;
  std::vector<std::vector<cv::Point2f>> imagePointsRight;
  
  std::vector<cv::Point2f> cornersLeft;
  std::vector<cv::Point2f> cornersRight;

  // load images from the specified destination to the vector
  loadImages("./img/left/", "bmp", 17, &leftImages);
  loadImages("./img/right/", "bmp", 17, &rightImages);

  std::cout << leftImages.size() << std::endl;
  std::cout << rightImages.size() << std::endl; 

  cv::imshow("goo", leftImages[6]);
  cv::waitKey(0);

#if 0
  for(int i = 0; i < numSquares; ++i)
    obj.push_back(cv::Point3f(i/hCorners, i%hCorners, 0.0f));

  //#pragma omp parallel for
  if(leftImages.size() == rightImages.size()) {

    for(unsigned int i = 0; i < leftImages.size(); ++i) {
      cv::Mat grayImageLeft;
      cv::cvtColor(leftImages[i], grayImageLeft, CV_BGR2GRAY);

#if 0
      cv::Mat grayImageRight;
      cv::cvtColor(rightImages[i], grayImageRight, CV_BGR2GRAY);
#endif
      cv::Size imageSize = leftImages[0].size();
     
      bool foundLeft = cv::findChessboardCorners(leftImages[i], boardSize, cornersLeft, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
      bool foundRight = cv::findChessboardCorners(rightImages[i], boardSize, cornersRight, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

      if(foundLeft && foundRight) {
        cv::cornerSubPix(leftImages[i], cornersLeft, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        cv::cornerSubPix(rightImages[i], cornersRight, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        //drawChessboardCorners(grayImage, boardSize, corners, found);
      }

      imagePointsLeft.push_back(cornersLeft);
      imagePointsRight.push_back(cornersRight);
      objectPoints.push_back(obj);

      // camera calibration
      // cv::calibrateCamera(objectPoints, imagePoints, colorImages[i].size(), intrinsic, distCoeffs, rvecs, tvecs);
      cv::stereoCalibrate(objectPoints, imagePointsLeft, imagePointsRight, intrinsicLeft, distCoeffsLeft, intrinsicRight, distCoeffsRight, imageSize, R, T, E, F);
#if 0
      grayImageLeft.release();
      grayImageRight.release();
#endif
    }

  } else {
    std::cout << "The Number of calibration images is not equal!" << std::endl;
  }

  // undistort the images
#if 0
  double start = omp_get_wtime();
  for(unsigned int i = 0; i < colorImages.size(); ++i) {
    cv::Mat undistorted;
    std::string imageNumber = std::to_string(i);

    // undistort the original images using the calculated cameraMatrix and the disortion Coefficients
    cv::undistort(colorImages[i], undistorted, intrinsic, distCoeffs);

    // write the undistorted Image to the out folder
    cv::imwrite("./out/"+ imageNumber + ".jpg", undistorted);
    undistorted.release();
  }
  double end = omp_get_wtime();

  saveCoefficients("coefficients.yml", intrinsic, distCoeffs, rvecs, tvecs);

#if 0
  for(unsigned int i = 0; i < colorImages.size(); ++i) {
    cv::imshow("foo", colorImages[i]);
    cv::waitKey(0);
  }
#endif

  std::cout << end - start << std::endl;

#endif
#endif
  return 0;
}
