
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main (int argc, char *argv[])
{
    char *filename;

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " [mp4 filename]" << endl;
        return 1;
    }

    filename = argv[1];

    VideoCapture cap(filename);

    if (!cap.isOpened()) {
        cout << "Cannot open video file " << filename << endl;
        return 2;
    }

    cout << "Video info:" << endl;
    cout << "\t frame size: " << cap.get(CAP_PROP_FRAME_WIDTH) << "x" << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "\t frame rate: " << cap.get(CAP_PROP_FPS) << endl;
    cout << "\t frame count: " << cap.get(CAP_PROP_FRAME_COUNT) << endl;

    cap.release();

    return 0;
}

