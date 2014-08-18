#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <stdio.h>
#include <sstream>
#include <string>

using namespace std;
using namespace cv;

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

//Things I want to do:
//Make a window for the HSV trackbars
//Make a window for the position in the video with button for play/pause

void on_HSV_trackbar( int position, void* ){
	
}

void createTrackbarsWindow(){
	string trackbarWindowName = "Adjust me!";
	namedWindow(trackbarWindowName,0);
	
	char TrackbarName[50];

	createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_HSV_trackbar);
	createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_HSV_trackbar);
	createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_HSV_trackbar);
	createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_HSV_trackbar);
	createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_HSV_trackbar);
	createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_HSV_trackbar);
}


int main(int argc, char* argv[]){
	//Declaring local variables for the main function
	string videoFileName = "juggle.mov";

	//Opening the video file
	VideoCapture video(videoFileName);
	if(!video.isOpened()){
		cout << "Cannot open the file" << endl;
		return -1;
	}

	//Creating the other windows
	createTrackbarsWindow();

  while(1){
		//Declaring the local variables for the video loop
    Mat frame, out, temp, gray;

		//Reads a new frame from the video and verifies it
    bool readVideoSuccess = video.read(frame);
    if (!readVideoSuccess){
      cout << "Cannot read from file" << endl;
      break;
    }
				
		//Where the image manipulation happens

				//cv::GaussianBlur( frame, out, Size(5,5), 3, 3);
				pyrDown(frame, temp);				
				cvtColor(temp, gray, CV_BGR2HSV);
        //imshow("MyVideo", frame); //show the frame in "MyVideo" window

				inRange(gray, Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),out);
				inRange(gray, Scalar(165,105,170),Scalar(230,256,205),out);
				//morphOps(out);

				vector<Vec3f> circles;

				HoughCircles(out, circles, CV_HOUGH_GRADIENT, 1, out.rows/2, 20, 10, 0, 0 );

				for( size_t i = 0; i < circles.size(); i++ )
				{
					Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
					int radius = cvRound(circles[i][2]);
					// draw the circle center
					circle( temp, center, 3, Scalar(0,255,0), -1, 8, 0 );
					// draw the circle outline
					circle( temp, center, radius, Scalar(0,0,255), 3, 8, 0 );
				}

        imshow("output", out); //show the frame in "MyVideo" window

				if(waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
       {
                cout << "esc key is pressed by user" << endl; 
                break; 
       }
    }

    return 0;

}
