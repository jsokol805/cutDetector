#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
#include <cstdio>
#include <list>
#include <algorithm>
#include <utility>
#include <vector>

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

#include <frame_comparator_impl.hpp>

using namespace std;
using namespace cv;

int main(int argc, char *argv[]) {
	VideoCapture videoFile("reklama.mp4");

	if( !videoFile.isOpened() ){
		std::cerr << "failed to open file" << std::endl;
		return -1;
	}

	FrameComparatorImpl* comparator = new FrameComparatorImpl();

	Size dimensions = Size(
			(int)videoFile.get(CV_CAP_PROP_FRAME_WIDTH),
			(int)videoFile.get(CV_CAP_PROP_FRAME_WIDTH));

	Mat currentFrame;
	Mat lastFrame;

	std::list<std::pair<int,int> > scenes;

	const char* window1 = "ostatnia klatka sceny";
	const char* window2 = "pierwsza klatka sceny";

	namedWindow(window1);
	namedWindow(window2);

	int lastFrameIndex, currentFrameIndex;
	currentFrameIndex = 0;
	int sceneStartIndex = 0;
	int totalFrames = videoFile.get(CV_CAP_PROP_FRAME_COUNT) - 1;

	videoFile.grab();
	videoFile.retrieve(currentFrame);

	while(1){
		lastFrame = currentFrame.clone();	

		lastFrameIndex = currentFrameIndex;
		currentFrameIndex = videoFile.get(CV_CAP_PROP_POS_FRAMES);

		videoFile >> currentFrame;

		if (currentFrameIndex == totalFrames) {
			break;
		}
		if (lastFrameIndex == 0) {
			continue;
		}
		if( comparator->isDifferentScene(lastFrame, currentFrame) ){
			std::cout << "[" << sceneStartIndex << ";" << lastFrameIndex << "]" << std::endl;
			scenes.push_back(std::make_pair(sceneStartIndex, currentFrameIndex));
			sceneStartIndex = currentFrameIndex;
			Mat lastFrameResized, currentFrameResized;
			resize(lastFrame,
				   lastFrameResized,
				   Size(),
				   0.25,
				   0.25,
				   CV_INTER_AREA);
			resize(currentFrame,
				   currentFrameResized,
				   Size(),
				   0.25,
				   0.25,
				   CV_INTER_AREA);
			imshow(window1, lastFrameResized);
			imshow(window2, currentFrameResized);
			waitKey(10000);
		}

	}

	scenes.push_back(std::make_pair(sceneStartIndex, currentFrameIndex));
	std::cout << "[" << sceneStartIndex << ";" << currentFrameIndex << "]" << std::endl;
	delete comparator;

	return 0;
}
