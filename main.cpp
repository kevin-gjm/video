#include "mainwindow.h"
#include <QApplication>
#include "facerecognition.hpp"

int main(int argc, char *argv[])
{
    char* path = "D:\\111.jpeg";
    cv::Mat img = cv::imread(path);
    FaceRecognition* recog = new FaceRecognition();
    recog->setImg(img);
    recog->recognition();

    cvWaitKey();

    return 0;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
