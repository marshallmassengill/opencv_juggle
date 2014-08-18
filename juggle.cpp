#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace std;
//using namespace cv;

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//Things I want to do:
//Make a window for the HSV trackbars
//Make a window for the position in the video with button for play/pause

void onHSVTrackbarSlide(int position, void*){
	
}

void createHSVTrackbarsWindow(){
	string trackbarWindowName = "Adjust me!";
	cv::namedWindow(trackbarWindowName,0);
	
	char TrackbarName[50];

	cv::createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, onHSVTrackbarSlide);
	cv::createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, onHSVTrackbarSlide);
}

void onPositionTrackbarSlide(int position, void*){
	
}

void createPositionTrackbarWindow(){
	
}

void manipulateImage(cv::Mat &imageIn, cv::Mat &imageOut){
	cv::Mat temp, gray;

	//cv::GaussianBlur( frame, out, Size(5,5), 3, 3);
	cv::pyrDown(imageIn, temp);				
	cv::cvtColor(temp, gray, CV_BGR2HSV);
	//imshow("MyVideo", frame); //show the frame in "MyVideo" window
	cv::inRange(gray, cv::Scalar(H_MIN,S_MIN,V_MIN),cv::Scalar(H_MAX,S_MAX,V_MAX),imageOut);
	//cv::inRange(gray, cv::Scalar(165,105,170),cv::Scalar(230,256,205),imageOut);
	//morphOps(out);
}

//rewrite!!!!
void morphOps(cv::Mat &thresh){
  //create structuring element that will be used to "dilate" and "erode" image.
  //the element chosen here is a 3px by 3px rectangle

  cv::Mat erodeElement = getStructuringElement( cv::MORPH_RECT,cv::Size(3,3));
  //dilate with larger element so make sure object is nicely visible
  cv::Mat dilateElement = getStructuringElement( cv::MORPH_ELLIPSE,cv::Size(8,8));

  cv::erode(thresh,thresh,erodeElement);
  cv::erode(thresh,thresh,erodeElement);

  cv::dilate(thresh,thresh,dilateElement);
  cv::dilate(thresh,thresh,dilateElement);
}



int main(int argc, char* argv[]){
	//Declaring local variables for the main function
	string videoFileName = "juggle.mov";

	//Opening the video file
	cv::VideoCapture video(videoFileName);
	if(!video.isOpened()){
		cout << "Cannot open the file" << endl;
		return -1;
	}

	//Creating the other windows
	createHSVTrackbarsWindow();

  while(1){
		//Declaring the local variables for the video loop
    cv::Mat frame, output;

		//Reads a new frame from the video and verifies it
    bool readVideoSuccess = video.read(frame);
    if (!readVideoSuccess){
      cout << "Cannot read from file" << endl;
      break;
    }
				
		//Where the image manipulation happens
		manipulateImage(frame, output);

		//wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
		if(cv::waitKey(30) == 27){
			cout << "esc key is pressed by user" << endl; 
			break; 
    }



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

        cv::imshow("output", output); //show the frame in "MyVideo" window

    }
    return 0;
}
