#ifndef FACERECOGNITION_HPP
#define FACERECOGNITION_HPP
#include <opencv2/opencv.hpp>

class FaceRecognition
{
private:
    cv::Mat m_mImg;
    char* face_cascade_name = "C:\\msys64\\mingw64\\share\\OpenCV\\haarcascades\\haarcascade_frontalface_alt.xml";
    char* eyes_cascade_name = "C:\\msys64\\mingw64\\share\\OpenCV\\haarcascades\\haarcascade_eye.xml";
    cv::CascadeClassifier face_cascade;
    cv::CascadeClassifier eyes_cascade;

public:
    FaceRecognition();
    ~FaceRecognition();
    void recognition();
    void setImg(cv::Mat mat);
};

FaceRecognition::FaceRecognition(){}

FaceRecognition::~FaceRecognition()
{
    delete face_cascade_name;
    delete eyes_cascade_name;
}

void FaceRecognition::setImg(cv::Mat mat)
{
    this->m_mImg = mat;
}

void FaceRecognition::recognition()
{
    //-- 1. Load the cascades
    if (!face_cascade.load(face_cascade_name)){ printf("--(!)Error loading face cascade\n"); return ; };
    if (!eyes_cascade.load(eyes_cascade_name)){ printf("--(!)Error loading eyes cascade\n"); return ; };

    std::vector<cv::Rect> faces;
    cv::Mat img_gray;

    cvtColor(m_mImg, img_gray, cv::COLOR_BGR2GRAY);
    cv::equalizeHist(img_gray, img_gray);

    //-- 2. Detect faces
    face_cascade.detectMultiScale(img_gray, faces, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

    for (int i = 0; i < faces.size(); i++)
    {
        cv::Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        ellipse(m_mImg, center, cv::Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);

        cv::Mat faceROI = img_gray(faces[i]);
        std::vector<cv::Rect> eyes;

        //-- In each face, detect eyes
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30));

        for (size_t j = 0; j < eyes.size(); j++)
        {
            cv::Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
            int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
            circle(m_mImg, eye_center, radius, cv::Scalar(255, 0, 0), 4, 8, 0);
        }
    }

    //-- 3. Show result
    cv::imshow("Res", m_mImg);
}
#endif // FACERECOGNITION_HPP
