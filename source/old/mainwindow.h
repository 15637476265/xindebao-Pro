#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cameraControl.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openport();
private:
    Ui::MainWindow *ui;
    QSerialPort * MainSerial;
    QByteArray MainSerialRecvData;
    AdjustCamera* _camera_ptr;
    bool m_is_open;
    QTimer* qTimer;
    bool btakePictures;
    void saveImg(const cv::Mat &mat, const std::string &path = "", const std::string &name = "");
private slots:
    void showPicture();
    void MainSerialRecvMsgEvent();
    void on_btnExp1_clicked();
    void on_pushButton_2_clicked();
    void on_btnGain1_clicked();
    void on_btnStopTake1_clicked();
    void on_btnStartTake1_clicked();
    void on_btnOpenClose1_clicked();
    void on_btnRoi1_clicked();
    void on_btnTakePicture1_clicked();
};

#endif // MAINWINDOW_H
