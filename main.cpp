//code based on Exercise-8
#include <GLFW/glfw3.h>
#include "DrawPrimitives.h"
#include <iostream>
#include <iomanip>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/persistence.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include<conio.h>
//#include "PoseEstimation.h"
//#include "MarkerTracker.h"

using namespace std;

cv::VideoCapture cap;

// Camera settings
const int camera_width = 640;
const int camera_height = 480;
const int virtual_camera_angle = 30;
unsigned char bkgnd[camera_width * camera_height * 3];

//global player attributer setting begin  //Helpless_guy
float x_movement = 0.0f; // player pos x
float y_movement = 0.0f;// player pos y
float z_movement = 0.0f;// player pos z
float foot_step = 0.1f; // player speed
float fast_foot_step = 0.3f;// player running speed
float current_foot_frequency_factor = 1.0f;// current palyer foot frequency
float fast_foot_frequency_factor = 6.0f;// palyer fast foot frequency
float foot_frequency_factor = 1.0f;// palyer normal foot frequency
float original_direction = -90.0f; //player's direction


float threshold_noise_x = 0.3; // if the variable x is under the threshold then we consider it as nosie and representing not moving
float threshold_fast_x = 0.5;// if the variable x under this threshold, the it represent walking; above the threshold ,then it's running.


float threshold_noise_y = 0.4;// if the variable y is under the threshold then we consider it as nosie and representing not moving
float threshold_fast_y = 0.6;// if the variable y under this threshold, the it represent walking; above the threshold ,then it's running.
float threshold_noise_z = 1;// if the variable y under this threshold, the it represent walking; above the threshold ,then it's running.
float threshold_fast_z = 2;// if the variable z under this threshold, the it represent walking; above the threshold ,then it's running.



enum direction {
	Non, East, West, North, South, FastEast, FastWest, FastNorth, FastSouth,
	EastNorth, WestNorth, EastSouth, WestSouth,
	FastEastNorth, FastWestNorth, FastEastSouth, FastWestSouth
};

direction currentDirection = Non;
float lastDirection = 0;
//global player attributer setting end


void initVideoStream(cv::VideoCapture& cap) {
	if (cap.isOpened())
		cap.release();

	cap.open(0);
	if (cap.isOpened() == false) {
		std::cout << "No webcam found, using a video file" << std::endl;
		cap.open("MarkerMovie.mpg");
		if (cap.isOpened() == false) {
			std::cout << "No video file found. Exiting." << std::endl;
			exit(0);
		}
	}
}

//OpenGL initialization 
void initGL(int argc, char* argv[]) {

	// For our connection between OpenCV/OpenGL
	// Pixel storage/packing stuff -> how to handle the pixel on the graphics card
	// For glReadPixels​ -> Pixel representation in the frame buffer
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// For glTexImage2D​ -> Define the texture image representation
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Turn the texture coordinates from OpenCV to the texture coordinates OpenGL
	glPixelZoom(1.0, -1.0);

	// Enable and set colors
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0, 0, 0, 1.0);

	// Enable and set depth parameters
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// Light parameters
	GLfloat light_amb[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light_pos[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_dif[] = { 0.7, 0.7, 0.7, 1.0 };

	// Enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_dif);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}


void display(GLFWwindow* window, const cv::Mat& img_bgr, float resultMatrix[16], direction dir) {
	
	// Copy picture data into bkgnd array
	memcpy(bkgnd, img_bgr.data, sizeof(bkgnd));

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Needed for rendering the real camera image
	glMatrixMode(GL_MODELVIEW);

	// No position changes
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	// Push the projection matrix (frustum) -> frustum will be saved on the stack
	glPushMatrix();

	glLoadIdentity();
	// In the ortho view all objects stay the same size at every distance

	glOrtho(0.0, camera_width, 0.0, camera_height, -1, 1);

	// -> Render the camera picture as background texture
	// 
	// Making a raster of the image -> -1 otherwise overflow
	glRasterPos2i(0, camera_height - 1);
	// Load and render the camera image -> unsigned byte because of bkgnd as unsigned char array
	// bkgnd 3 channels -> pixelwise rendering
	glDrawPixels(camera_width, camera_height, GL_BGR_EXT, GL_UNSIGNED_BYTE, bkgnd);

	// Go back to the previous projection -> frustum
	glPopMatrix();

	// Activate depth -> that snowman can be scaled with depth
	glEnable(GL_DEPTH_TEST);

	// Move to marker-position
	glMatrixMode(GL_MODELVIEW);

	// -> Transpose the Modelview Matrix
	float resultTransposedMatrix[16];
	for (int x = 0; x < 4; ++x) {
		for (int y = 0; y < 4; ++y) {
			// Change columns to rows
			resultTransposedMatrix[x * 4 + y] = resultMatrix[y * 4 + x];
		}
	}

	// Load the transpose matrix
	glLoadMatrixf(resultTransposedMatrix);

	//position correction
	glTranslatef(-0.05, 0.04, 0);

	// Rotate 90 desgress in x-direction
	glRotatef(90, 1, 0, 0);

	// Scale down
	glScalef(0.03, 0.03, 0.03);

	switch (dir) {
	case East:
		x_movement = x_movement + foot_step;
		break;
	case West:
		x_movement = x_movement - foot_step;
		break;
	case North:
		z_movement = z_movement + foot_step;
		break;
	case South:
		z_movement = z_movement - foot_step;
		break;

	case EastNorth:
		x_movement = x_movement + foot_step;
		z_movement = z_movement + foot_step;
		break;
	case WestNorth:
		x_movement = x_movement - foot_step;
		z_movement = z_movement + foot_step;
		break;

	case EastSouth:
		x_movement = x_movement + foot_step;
		z_movement = z_movement - foot_step;
		break;
	case WestSouth:
		x_movement = x_movement - foot_step;
		z_movement = z_movement - foot_step;
		break;


	case FastEast:
		x_movement = x_movement + fast_foot_step;
		break;
	case FastWest:
		x_movement = x_movement - fast_foot_step;
		break;
	case FastNorth:
		z_movement = z_movement + fast_foot_step;
		break;
	case FastSouth:
		z_movement = z_movement - fast_foot_step;
		break;

	case FastEastNorth:
		x_movement = x_movement + foot_step;
		z_movement = z_movement + foot_step;
		break;
	case FastWestNorth:
		x_movement = x_movement - foot_step;
		z_movement = z_movement + foot_step;
		break;

	case FastEastSouth:
		x_movement = x_movement + foot_step;
		z_movement = z_movement - foot_step;
		break;
	case FastWestSouth:
		x_movement = x_movement - foot_step;
		z_movement = z_movement - foot_step;
		break;
	case Non:
		break;

	}
	//get direction input and make movement  end
	// -> Presettings which will be used for the objects which are created afterwards

	// Move the object backwards
	//glTranslatef(0.0, -0.8, -10.0);

	glTranslatef(x_movement, 0, z_movement);

	// [degree/sec]

	//	change face direction begin  Helpless_guy
	const float degreePerSec = 90.0f;

	/// glfwGetTime returns elapsed time counted from the program start in second
	const float angle = (float)glfwGetTime() * degreePerSec;

	switch (dir) {
	case East:
		//x_movement = x_movement + foot_step;
		glRotatef(original_direction + 90.0f, 0, 1, 0);
		lastDirection = original_direction + 90.0f;
		break;

	case West:
		//x_movement = x_movement - foot_step;
		glRotatef(original_direction - 90.0f, 0, 1, 0);
		lastDirection = original_direction - 90.0f;
		break;

	case North:
		//y_movement = y_movement + foot_step;
		glRotatef(original_direction, 0, 1, 0);
		lastDirection = original_direction;
		//std::cout << "goNorth";
		break;

	case South:
		//y_movement = y_movement - foot_step;
		glRotatef(original_direction + 180.0f, 0, 1, 0);
		//std::cout << "goSouth";
		lastDirection = original_direction + 180.0f;
		break;

	case FastEast:
		//x_movement = x_movement + fast_foot_step;
		glRotatef(original_direction + 90.0f, 0, 1, 0);
		lastDirection = original_direction + 90.0f;
		//std::cout << "goFastEast";
		break;

	case FastWest:
		//x_movement = x_movement - fast_foot_step;
		glRotatef(original_direction - 90.0f, 0, 1, 0);
		lastDirection = original_direction - 90.0f;
		//std::cout << "goFastWest";
		break;

	case FastNorth:
		//y_movement = y_movement + fast_foot_step;
		glRotatef(original_direction, 0, 1, 0);
		lastDirection = original_direction;
		//std::cout << "goFastNorth";
		break;

	case FastSouth:
		//y_movement = y_movement - fast_foot_step;
		glRotatef(original_direction + 180.0f, 0, 1, 0);
		lastDirection = original_direction + 180.0f;
		//std::cout << "goFastSouth";
		break;

	case EastNorth:
		glRotatef(original_direction + 45.0f, 0, 1, 0);
		lastDirection = original_direction + 45.0f;
		//std::cout << "goEast";
		break;

	case WestNorth:
		glRotatef(original_direction - 45.0f, 0, 1, 0);
		lastDirection = original_direction - 45.0f;
		//std::cout << "goWest";
		break;

	case EastSouth:
		glRotatef(original_direction + 135.0f, 0, 1, 0);
		lastDirection = original_direction + 135.0f;
		//std::cout << "goEast";
		break;

	case WestSouth:
		glRotatef(original_direction - 135.0f, 0, 1, 0);
		lastDirection = original_direction - 135.0f;
		//std::cout << "goWest";
		break;

	case FastEastNorth:
		glRotatef(original_direction + 45.0f, 0, 1, 0);
		lastDirection = original_direction + 45.0f;
		//std::cout << "goEast";
		break;

	case FastWestNorth:
		glRotatef(original_direction - 45.0f, 0, 1, 0);
		lastDirection = original_direction - 45.0f;
		//std::cout << "goWest";
		break;

	case FastEastSouth:
		glRotatef(original_direction + 135.0f, 0, 1, 0);
		lastDirection = original_direction + 135.0f;
		//std::cout << "goEast";
		break;

	case FastWestSouth:
		glRotatef(original_direction - 135.0f, 0, 1, 0);
		lastDirection = original_direction - 135.0f;
		//std::cout << "goWest";
		break;

	case Non:
		//y_movement = y_movement - fast_foot_step;
		glRotatef(lastDirection, 0, 1, 0);
		//std::cout << "goFastSouth";
		break;

	}

	//	change face direction end  Helpless_guy

	// generate player Helpless_guy
		// Draw 3 white spheres
	
	// Draw body
	glColor4f(1.0, 1.0, 1, 1.0);

	//drawSphere(0.8, 10, 10);
	glRotatef(90, 1, 0, 0);
	drawCone(0.5, 1, 10, 10);
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0.0, 0.4, 0.0);
	glColor4f(1.0, 1.0, 1, 1.0);
	drawSphere(0.4, 10, 10);

	// Draw the eyes
	// Push -> save the pose (in a modelview matrix)
	glPushMatrix();
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glTranslatef(0.2, 0.2, 0.2);
	drawSphere(0.066, 10, 10);
	glTranslatef(0, 0, -0.4);
	drawSphere(0.066, 10, 10);

	// Pop -> go back to the last saved pose
	glPopMatrix();
	glPushMatrix();

	// Draw a nose
	glColor4f(1.0, 0.5, 0.0, 1.0);
	glTranslatef(0.3, 0.0, 0.0);

	// The cone needs to be rotated in y-direction because the tip is pointing backwards
	glRotatef(90, 0, 1, 0);
	drawCone(0.1, 0.3, 10, 10);

	//// Draw the foots
	// Push -> save the pose (in a modelview matrix)
	glPopMatrix();
	glPushMatrix();

	// intial front foot position glTranslatef(1, -1.5, 0.5);
	//intial back foot position 	glTranslatef(-2.0, 0, -0.8);
	glTranslatef(0 - 1 * sin((float)glfwGetTime() * current_foot_frequency_factor), -1.5 + 0.5 - cos((float)glfwGetTime() * current_foot_frequency_factor), 0.5);
	//glTranslatef(0 - 1 * cos((float)glfwGetTime()), -1.5 + 0.5 - sin((float)glfwGetTime()), 0.5);
	//glTranslatef(0 , -1.5 + 0.5 - sin((float)glfwGetTime()), 0.5 - 1 * cos((float)glfwGetTime()));
	drawPOLYGO();

	glPopMatrix();
	glTranslatef(-1.0 - 1 * sin((float)glfwGetTime() * current_foot_frequency_factor + 10), -1.5 - 0.5 * cos((float)glfwGetTime() * current_foot_frequency_factor + 10), -0.5);
	//glTranslatef(-1.0 - 1 * cos((float)glfwGetTime() + 10), -1.5 - 0.5 * sin((float)glfwGetTime() + 10), -0.5);
	//glTranslatef(-1.0 , -1.5 - 0.5 * sin((float)glfwGetTime() + 10), -0.5 - 1 * cos((float)glfwGetTime() + 10));
	drawPOLYGO();
	// generate player end  Helpless_guy
}


void reshape(GLFWwindow* window, int width, int height) {
	// Set a whole-window viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// Create a perspective projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// The camera should be calibrated -> a calibration results in the projection matrix -> then load the matrix
	// -> into GL_PROJECTION
	// -> adjustment of FOV is needed for each camera
	float ratio = (GLfloat)width / (GLfloat)height;

	float near = 0.01f, far = 100.f;
	float top = tan((double)(virtual_camera_angle * M_PI / 360.0f)) * near;
	float bottom = -top;
	float left = ratio * bottom;
	float right = ratio * top;
	glFrustum(left, right, bottom, top, near, far);
}

// calculate new direction from dx,dy,dz,get enum
direction get_New_direction(float trans_x, float trans_y, float trans_z) {

	std::cout << "trans_x:" << trans_x << " ; trans_y: " << trans_y << " ; trans_z:" << trans_z << endl;
	int isX = 0;
	int isY = 0;
	int isZ = 0;
	int isFastX = 0;
	int isFastY = 0;
	int isFastZ = 0;
	direction ArrayDirection[9] = { North,EastNorth,East,EastSouth, South,WestSouth,West ,WestNorth,Non };
	direction FastArrayDirection[9] = { FastNorth,FastEastNorth,FastEast,FastEastSouth, FastSouth,FastWestSouth,FastWest ,FastWestNorth,Non };
	direction result = Non;

	/*
		Non, East, West, North, South, FastEast, FastWest, FastNorth, FastSouth,
		EastNorth, WestNorth, EastSouth, WestSouth,
		FastEastNorth, FastWestNorth, FastEastSouth, FastWestSouth

		*/
		// moving towards righy, then tans_x has a negative value.
		// moving upwards, then tans_y has a negative value.
		//moving upwards, then trans_z has a positive value.


	if (abs(trans_x) > threshold_noise_x) {
		isX = 1;
		std::cout << "isX => 1";
	}
	else { trans_x = 0; }

	if (abs(trans_x) > threshold_fast_x) {
		std::cout << "isFastX => 1";
		isFastX = 1;
	}

	if (abs(trans_y) > threshold_noise_y) {
		isY = 1;
		std::cout << "isY => 1";
	}
	else { trans_y = 0; }

	if (abs(trans_y) > threshold_noise_y) {
		std::cout << "isFastY => 1";
		isFastY = 1;
	}

	if (abs(trans_z) > threshold_noise_z) {
		isZ = 1;
		std::cout << "isY => 1";
	}
	else { trans_z = 0; }
	if (abs(trans_z) > threshold_fast_z) {
		isFastZ = 1;
		std::cout << "isFastZ => 1";
	}
	std::cout << "isX:" << isX << " ; isY: " << isY << " ; isZ:" << isZ << endl;




	// if there is running in any direction, then it should be catagorized as running.
	// moving forward (trans_y < 0) is so to speak moving north., 
	// We denote value 0 as north, other directions  has a clocwise notation. Then East has a value of 2 and West has a value of 6.
	if (isFastX == 0 && isFastY == 0 && isFastZ == 0) {
		std::cout << " not Fast" << endl;
		std::cout << "new trans_x:" << trans_x << " ;new trans_y: " << trans_y << " ;new trans_z:" << trans_z << endl;

		if (trans_y < 0 && trans_x == 0) { result = ArrayDirection[0]; }
		if (trans_y < 0 && trans_x < 0) { result = ArrayDirection[1]; }
		if (trans_y == 0 && trans_x < 0) { result = ArrayDirection[2]; }
		if (trans_y > 0 && trans_x < 0) { result = ArrayDirection[3]; }
		if (trans_y > 0 && trans_x == 0) { result = ArrayDirection[4]; }
		if (trans_y > 0 && trans_x > 0) { result = ArrayDirection[5]; }
		if (trans_y == 0 && trans_x < 0) { result = ArrayDirection[6]; }
		if (trans_y < 0 && trans_x > 0) { result = ArrayDirection[7]; }
		if (trans_z != 0) { result = ArrayDirection[8]; }
	}
	else {
		std::cout << " Fast" << endl;
		std::cout << "new Fast trans_x:" << trans_x << " ;new Fast trans_y: " << trans_y << " ;new Fast trans_z:" << trans_z << endl;

		if (trans_y < 0 && trans_x == 0) { result = FastArrayDirection[0]; }
		if (trans_y < 0 && trans_x < 0) { result = FastArrayDirection[1]; }
		if (trans_y == 0 && trans_x < 0) { result = FastArrayDirection[2]; }
		if (trans_y > 0 && trans_x < 0) { result = FastArrayDirection[3]; }
		if (trans_y > 0 && trans_x == 0) { result = FastArrayDirection[4]; }
		if (trans_y > 0 && trans_x > 0) { result = FastArrayDirection[5]; }
		if (trans_y == 0 && trans_x < 0) { result = FastArrayDirection[6]; }
		if (trans_y < 0 && trans_x > 0) { result = FastArrayDirection[7]; }
		if (trans_z != 0) { result = FastArrayDirection[8]; }
	}
	std::cout << "result:" << result << endl;


	return result;
}


int main(int argc, char* argv[]) {
	GLFWwindow* window;

	// Initialize the library
	if (!glfwInit())
		return -1;

	// Initialize the window system
	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(camera_width, camera_height, "Exercise 8 - Combine", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	// Set callback functions for GLFW
	glfwSetFramebufferSizeCallback(window, reshape);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	int window_width, window_height;
	glfwGetFramebufferSize(window, &window_width, &window_height);
	reshape(window, window_width, window_height);

	// Initialize the GL library
	initGL(argc, argv);

	// Setup OpenCV
	cv::Mat img_bgr;
	// Get video stream
	initVideoStream(cap);

	//set marker id dict
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	//read cameramatrix and distcoefficients
	cv::Mat cameraMatrix, distCoeffs;
	cv::FileStorage fs("camera.yml", 0);

	if (!fs.isOpened())
	{
		std::cout << "Could not open the configuration file!" << std::endl;
		exit(1);
	}

	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> distCoeffs;
	fs.release();
	std::cout << cameraMatrix << std::endl;
	std::cout << distCoeffs << std::endl;

	//for print test
	int count = 0;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {

		// Capture 
		cap >> img_bgr;
		cv::Mat img_bgr_copy;
		img_bgr.copyTo(img_bgr_copy);

		std::vector<int> ids;
		std::vector<std::vector<cv::Point2f> > corners;
		cv::aruco::detectMarkers(img_bgr, dictionary, corners, ids);//24 - world, 26 - marker

		if (img_bgr.empty()) {
			std::cout << "Could not query frame. Trying to reinitialize." << std::endl;
			initVideoStream(cap);
			// Wait for one sec.
			cv::waitKey(1000);
			continue;
		}

		float pose_matrix[16];

		if (ids.size() > 1) {
			//pose estimation
			std::vector<cv::Vec3d> tvecs, rvecs; //rotation vector - Rodrigues

			cv::aruco::drawDetectedMarkers(img_bgr, corners, ids);
			cv::aruco::estimatePoseSingleMarkers(corners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

			cv::Mat base; //base coord
			cv::Mat rotation; //marker coord
			int i = 0;
			if (ids[1] == 26) {

				cv::Rodrigues(rvecs[1], rotation);
				cv::Rodrigues(rvecs[0], base);

				pose_matrix[0] = (float)rotation.at<double>(0, 0);
				pose_matrix[1] = (float)rotation.at<double>(0, 1);
				pose_matrix[2] = (float)rotation.at<double>(0, 2);
				pose_matrix[3] = (float)(tvecs[1][0]);
				pose_matrix[4] = -1.0 * (float)rotation.at<double>(1, 0);
				pose_matrix[5] = -1.0 * (float)rotation.at<double>(1, 1);
				pose_matrix[6] = -1.0 * (float)rotation.at<double>(1, 2);
				pose_matrix[7] = -1.0 * (float)(tvecs[1][1]);
				pose_matrix[8] = -1.0 * (float)rotation.at<double>(2, 0);
				pose_matrix[9] = -1.0 * (float)rotation.at<double>(2, 1);
				pose_matrix[10] = -1.0 * (float)rotation.at<double>(2, 2);
				pose_matrix[11] = -1.0 * (float)(tvecs[1][2]);;
				pose_matrix[12] = 0;
				pose_matrix[13] = 0;
				pose_matrix[14] = 0;
				pose_matrix[15] = 1;

			}

			else {

				cv::Rodrigues(rvecs[0], rotation);
				cv::Rodrigues(rvecs[1], base);

				pose_matrix[0] = (float)rotation.at<double>(0, 0);
				pose_matrix[1] = (float)rotation.at<double>(0, 1);
				pose_matrix[2] = (float)rotation.at<double>(0, 2);
				pose_matrix[3] = (float)(tvecs[0][0]);
				pose_matrix[4] = -1.0 * (float)rotation.at<double>(1, 0);
				pose_matrix[5] = -1.0 * (float)rotation.at<double>(1, 1);
				pose_matrix[6] = -1.0 * (float)rotation.at<double>(1, 2);
				pose_matrix[7] = -1.0 * (float)(tvecs[0][1]);
				pose_matrix[8] = -1.0 * (float)rotation.at<double>(2, 0);
				pose_matrix[9] = -1.0 * (float)rotation.at<double>(2, 1);
				pose_matrix[10] = -1.0 * (float)rotation.at<double>(2, 2);
				pose_matrix[11] = -1.0 * (float)(tvecs[0][2]);;
				pose_matrix[12] = 0;
				pose_matrix[13] = 0;
				pose_matrix[14] = 0;
				pose_matrix[15] = 1;
			}

			//distance
			cv::Vec3d trans = tvecs[1] - tvecs[0];
			float x_trans = trans.dot(base.col(0));
			float y_trans = trans.dot(base.col(1));
			float z_trans = trans.dot(base.col(2));

			//print for test
			/*if (!(count % 200))
			{
				std::cout << "trans  "<< trans << std::endl;
				std::cout << "x_trans  "<< x_trans << std::endl;
				std::cout << "y_trans  " << y_trans << std::endl;
				std::cout << "z_trans  " << z_trans << std::endl;
			}*/

			//draw Axes, X: red, Y: green, Z: blue
			for (int i = 0; i < rvecs.size(); ++i) {
				auto rvec = rvecs[i];
				auto tvec = tvecs[i];
				cv::drawFrameAxes(img_bgr, cameraMatrix, distCoeffs, rvec, tvec, 0.1);
			}

			/*for (int a = 0; a < 15; a++) {
				display(window, img_bgr, pose_matrix, currentDirection);
				currentDirection = get_New_direction(x_trans, y_trans, z_trans);
			}*/

			display(window, img_bgr, pose_matrix, currentDirection);
			currentDirection = get_New_direction(x_trans, y_trans, z_trans);

		}

		cv::imshow("out", img_bgr_copy);

		// Track a marker and get the pose of the marker
		//markerTracker.findMarker(img_bgr, resultMatrix_my);

		// Render here
		//display(window, img_bgr, pose_matrix,currentDirection);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		// get command from keyboard begin Helpless_guy for test

		if (_kbhit())//get user input for test  
		{
			currentDirection = Non;
			char cTake = _getche();
			std::cout << cTake;
			switch (cTake) {
			case 'd':
				currentDirection = West;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'a':
				currentDirection = East;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'w':
				currentDirection = North;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'x':
				currentDirection = South;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'e':
				currentDirection = WestNorth;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'q':
				currentDirection = EastNorth;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'z':
				currentDirection = EastSouth;
				current_foot_frequency_factor = foot_frequency_factor;
				break;
			case 'c':
				currentDirection = WestSouth;
				current_foot_frequency_factor = foot_frequency_factor;
				break;

				//faster speed;
			case 'j':
				currentDirection = FastWest;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 'g':
				currentDirection = FastEast;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 'y':
				currentDirection = FastNorth;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 'n':
				currentDirection = FastSouth;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 'u':
				currentDirection = FastWestNorth;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 't':
				currentDirection = FastEastNorth;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 'b':
				currentDirection = FastEastSouth;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			case 'm':
				currentDirection = FastWestSouth;
				current_foot_frequency_factor = fast_foot_frequency_factor;
				break;
			}
			//		get command from keyboard end Helpless_guy
		}

		count++;
		cv::waitKey(100);
	}

	// Important -> Avoid memory leaks!
	glfwTerminate();

	return 0;
}
