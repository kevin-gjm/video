#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // m_thread = new std::thread(std::bind(&MainWindow::HandleVideo,this,"D:\\123.mp4"));
    connect(this,SIGNAL(NewImage(QImage)),this,SLOT(GetNewImage(QImage)));
    connect(this,SIGNAL(DetectImage(QImage)),this,SLOT(DetectAndDisplay(QImage)));
    QPalette pal=ui->widgetVideo->palette();

    //设置背景黑色
    pal.setColor(QPalette::Background, Qt::black);
    ui->widgetVideo->setAutoFillBackground(true);
    ui->widgetVideo->setPalette(pal);

//    CascadeClassifier a;     //创建脸部对象
//    if(!a.load(sXmlPath))     //如果读取文件不出错，则检测人脸
//    {
//        cout<<"无法加载xml文件"<<endl;
//    }
    test();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DetectAndDisplay(QImage oQimg)
{
    oQimg.load("D:\\111.jpeg");
    Mat image = Util::QImage2cvMat(oQimg);
    string sXmlPath="C:\\msys64\\mingw64\\share\\OpenCV\\haarcascades\\haarcascade_frontalface_default.xml";
    CascadeClassifier ccf;      //创建脸部对象
    ccf.load(sXmlPath);           //导入opencv自带检测的文件
    vector<Rect> faces;
    Mat gray;
    cvtColor(image,gray,CV_BGR2GRAY);
    equalizeHist(gray,gray);
    ccf.detectMultiScale(gray,faces,1.1,3,0,Size(50,50),Size(500,500));
    for(vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
    {
        rectangle(image,*iter,Scalar(0,0,255),2,8); //画出脸部矩形
    }
    Mat image1;

    for(size_t i=0;i<faces.size();i++)
    {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        image1= image(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
        QImage img = Util::cvMat2QImage(image1);
        SetImage(img,ui->widgetInfo);
        cout<<"out img"<<endl;
    }


}

void MainWindow::GetNewImage(QImage img)
{
    SetImage(img,ui->widgetVideo);
}

void MainWindow::SetImage(QImage& img,QWidget* widget)
{
    widget->setAutoFillBackground(true);
    QPalette pall = widget->palette();
    pall.setBrush(QPalette::Window,QBrush(QPixmap::fromImage(img).scaled(widget->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    widget->setPalette(pall);
}


int MainWindow::saveToJPEG(AVCodecContext *pCodecCtx, AVFrame *pFrame,int index) {
    AVCodecContext *codecCtx;
    AVCodec *codec;

    int ret;
   // FILE *file = NULL;

    int y_size = pCodecCtx->width * pCodecCtx->height;
    AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据


    codec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    if (!codec) {
        printf("error avcodec_find_encoder ");
        return (0);
    }

    codecCtx = avcodec_alloc_context3(codec);
    if (!codecCtx) {
        printf("error avcodec_alloc_context3 ");
        return (0);
    }

    codecCtx->bit_rate = pCodecCtx->bit_rate;
    codecCtx->width = pCodecCtx->width;
    codecCtx->height = pCodecCtx->height;
    codecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    codecCtx->codec_id = AV_CODEC_ID_MJPEG;
    codecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    codecCtx->time_base.num = pCodecCtx->time_base.num;
    codecCtx->time_base.den = pCodecCtx->time_base.den;

    if (avcodec_open2(codecCtx, codec,NULL) < 0) {
        printf("error avcodec_open2 ");
        return (0);
    }

    codecCtx->mb_lmin = codecCtx->qmin * FF_QP2LAMBDA;
    codecCtx->mb_lmax = codecCtx->qmax * FF_QP2LAMBDA;
    codecCtx->flags = CODEC_FLAG_QSCALE;
    codecCtx->global_quality = codecCtx->qmin * FF_QP2LAMBDA;

    pFrame->pts = 1;
    pFrame->quality = codecCtx->global_quality;
    int got_frame = 0;
    ret = avcodec_encode_video2(codecCtx, packet, pFrame,&got_frame );
    int ii=0;
    if(got_frame)
    {
//        char szFilename[32];
//        sprintf(szFilename, "frame%d.jpg", index);
//        file = fopen(szFilename, "wb");
//        if (!file) {
//            printf("error fopen %s ",szFilename);
//            avcodec_close(codecCtx);
//            return (0);
//        }




//        fwrite(packet->data, 1, packet->size, file);
//        fclose(file);
        // QImage tmpImg(&packet->data[0],pCodecCtx->width,pCodecCtx->height,QImage::Format_ARGB32);
        ii++;
        QImage tmpImg;
        tmpImg.loadFromData(packet->data,packet->size);
      //  tmpImg.load(szFilename);
        emit NewImage(tmpImg);  //发送信号
        if(ii==1)
        {
             emit DetectImage(tmpImg);
        }

    }


    av_free_packet(packet);

    avcodec_close(codecCtx);
    avcodec_free_context(&codecCtx);

    return (ret);
}

int MainWindow::HandleVideo(string sVideoPath)
{
    try
    {
        int ret, got_picture;
        cout << "Hello FFmpeg!" << endl;
        av_register_all();
        unsigned version = avcodec_version();
        cout << "version is:" << version;

        AVFormatContext *pFormatCtx = avformat_alloc_context();

        if(0 != avformat_open_input(&pFormatCtx,sVideoPath.c_str(),NULL,NULL))
        {
            cout << "failed";
        }
        if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
            printf("Could't find stream infomation.");
            return -1;
        }

        ///循环查找视频中包含的流信息，直到找到视频类型的流
        ///便将其记录下来 保存到videoStream变量中
        ///这里我们现在只处理视频流  音频流先不管他
        int videoStream = -1;
        for (int i = 0; i < pFormatCtx->nb_streams; i++) {
            if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream = i;
            }
        }

        ///如果videoStream为-1 说明没有找到视频流
        if (videoStream == -1) {
            printf("Didn't find a video stream.  ");
        }

        AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
        AVCodec*  pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

        if (pCodec == NULL) {
            printf("Codec not found.");
            return -1;
        }

        ///打开解码器
        if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
            printf("Could not open codec.");
            return -1;
        }
        AVFrame* pFrame;
        pFrame = av_frame_alloc();


        int y_size = pCodecCtx->width * pCodecCtx->height;
        AVPacket *packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
        av_new_packet(packet, y_size); //分配packet的数据

        av_dump_format(pFormatCtx, 0, sVideoPath.c_str(), 0); //输出视频信息
        int index = 0;

        while (1)
        {
            if (av_read_frame(pFormatCtx, packet) < 0)
            {
                break; //这里认为视频读取完了
            }

            if (packet->stream_index == videoStream) {
                ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture,packet);

                if (ret < 0) {
                    printf("decode error. ");
                    return -1;
                }

                if (got_picture) {
                    index++;
                    if(index<1000) continue;
                    //if(index%50==0)
                    {
                        saveToJPEG(pCodecCtx,pFrame,index);
                    }
                    if (index > 30000) break; //这里我们就保存50张图片

                }
            }
            av_free_packet(packet);
        }

        av_free(pFrame);

        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
        avformat_free_context(pFormatCtx);
    }catch(exception& ex)
    {
        cout<<ex.what();
    }
}

void detectAndDisplay(Mat image)
{
    CascadeClassifier ccf;      //创建脸部对象
     string xml ="C:\\msys64\\mingw64\\share\\OpenCV\\haarcascades\\haarcascade_frontalface_default.xml";
    ccf.load(xml);           //导入opencv自带检测的文件
    vector<Rect> faces;
    Mat gray;
    cvtColor(image,gray,CV_BGR2GRAY);
    equalizeHist(gray,gray);
    ccf.detectMultiScale(gray,faces,1.1,3,0,Size(50,50),Size(500,500));
    for(vector<Rect>::const_iterator iter=faces.begin();iter!=faces.end();iter++)
    {
        rectangle(image,*iter,Scalar(0,0,255),2,8); //画出脸部矩形
    }
    Mat image1;

    for(size_t i=0;i<faces.size();i++)
    {
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        image1= image(Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height));
    }

    imshow("1",image);
    imshow("2",image1);
    cvWaitKey(0);

}

void MainWindow::test()
{
    string path="D:\\111.jpeg";//以检测图片1.jpg为例
    Mat image =imread(path,-1);
    string xml ="C:\\msys64\\mingw64\\share\\OpenCV\\haarcascades\\haarcascade_frontalface_default.xml";
    CascadeClassifier a;     //创建脸部对象
    if(!a.load(xml))     //如果读取文件不出错，则检测人脸
    {
        cout<<"无法加载xml文件"<<endl;
        return ;
    }
    detectAndDisplay(image);// 检测人脸
}
