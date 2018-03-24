#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include <thread>
#include <string>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/face.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/frame.h"
}


#include "util.h"

using namespace std;
using namespace cv;






namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    NewImage(QImage);
    DetectImage(QImage);
private slots:
    void GetNewImage(QImage);
    void DetectAndDisplay(QImage oQimg);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:

    int HandleVideo(string sVideoPath);
    int saveToJPEG(AVCodecContext *pCodecCtx, AVFrame *pFrame,int index);
    void SetImage(QImage& img,QWidget* widget);
    void test();

private:
    Ui::MainWindow *ui;
    std::thread *m_thread;
};

#endif // MAINWINDOW_H
