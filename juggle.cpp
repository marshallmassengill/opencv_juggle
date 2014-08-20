#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace std;
//using namespace cv;

int H_MIN = 160;
int H_MAX = 170;
int S_MIN = 100;
int S_MAX = 200;
int V_MIN = 140;
int V_MAX = 200;

cv::VideoCapture video;

void onHSVTrackbarSlide(int position, void*){
	
}

void createHSVTrackbarsWindow(){
	string trackbarWindowName = "Adjust HSV Parameters";
	cv::namedWindow(trackbarWindowName,cv::WINDOW_AUTOSIZE);

	cv::createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, onHSVTrackbarSlide);
}

void morphOps(cv::Mat &image){
  cv::Mat erodeElement = getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
  //dilate with larger element so make sure object is nicely visible
  cv::Mat dilateElement = getStructuringElement( cv::MORPH_ELLIPSE,cv::Size(11,11));

  cv::erode(image,image,erodeElement);
  cv::erode(image,image,erodeElement);

  cv::dilate(image,image,dilateElement);
  cv::dilate(image,image,dilateElement);
}

void makeCircles(cv::Mat &image){
	cv::gpu::GpuMat gpuCircleImage(image);
	cv::vector<cv::Vec3f> circles;

  //cv::HoughCircles(image, circles, CV_HOUGH_GRADIENT, 1, image.rows/2, 20, 10, 0, 0 );
  cv::gpu::HoughCircles(gpuCircleImage, circles, CV_HOUGH_GRADIENT, 1, gpuCircleImage.rows/2, 20, 10, 0, 0 );

  for( size_t i = 0; i < circles.size(); i++ ){
    cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // draw the circle center
    circle( gpuCircleImage, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
    // draw the circle outline
    circle( gpuCircleImage, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
  }
	image = cv::Mat(gpuCircleImage);
}

void manipulateImage(cv::Mat &imageIn, cv::Mat &imageOut){
	cv::gpu::GpuMat gpuImageIn(imageIn);
	cv::gpu::GpuMat gpuImageOut;
	cv::gpu::GpuMat gpuShrink, gpuGray;

	//cv::GaussianBlur( frame, out, Size(5,5), 3, 3);
	//cv::pyrDown(imageIn, temp);				
	//cv::pyrDown(imageIn, imageOut);				
	//cv::gpu::pyrDown(gpuImageIn,gpuShrink);
	cv::gpu::cvtColor(gpuImageIn, gpuGray, CV_BGR2HSV);
	//imshow("MyVideo", frame); //show the frame in "MyVideo" window
	cv::Mat cpuGray(gpuGray);
	//cv::inRange(cpuGray, cv::Scalar(H_MIN,S_MIN,V_MIN),cv::Scalar(H_MAX,S_MAX,V_MAX),imageOut);
	cv::inRange(cpuGray, cv::Scalar(160,105,170),cv::Scalar(170,210,205),imageOut);
	morphOps(imageOut);
	//imageOut = cv::Mat(gpuGray);
	//imageOut = cv::Mat(gpuImageOut);
}

void openVideoFile(){
	string videoFileName = "juggle.mov";
	
	video.open(videoFileName);
	if(!video.isOpened()){
		cout << "Cannot open the file" << endl;
		//return -1;
	}
}

int main(int argc, char* argv[]){
	//Declaring local variables for the main function
	bool run = true;

	//Opening the video file
	openVideoFile();

	//Creating the other windows
	createHSVTrackbarsWindow();

  while(1){
		//Declaring the local variables for the video loop
    cv::Mat frame, output;

		if(run){
			//Reads a new frame from the video and verifies it
			bool readVideoSuccess = video.read(frame);
			if (!readVideoSuccess){
				cout << "Cannot read from file" << endl;
      break;
			}
		
			//Where the image manipulation happens
			manipulateImage(frame, output);
			cv::imshow("output", output); //show the frame in "MyVideo" window
		}

		//wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		if(cv::waitKey(30) == 27){
			cout << "esc key is pressed by user" << endl; 
			break; 
    }

		//pause video if 'space' is pressed
		if(cv::waitKey(10) == 32){
				run = !run;
    }

    //cv::imshow("output", output); //show the frame in "MyVideo" window
		/*
				cv::vector<cv::Vec3f> circles;

				cv::HoughCircles(output, circles, CV_HOUGH_GRADIENT, 1, output.rows/2, 20, 10, 0, 0 );

				for( size_t i = 0; i < circles.size(); i++ )
				{
					cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
					int radius = cvRound(circles[i][2]);
					// draw the circle center
					circle( output, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
					// draw the circle outline
					circle( output, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
				}
		*/
    }
    return 0;
}
