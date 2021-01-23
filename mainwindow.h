#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFileDialog"
#include "inputreader.h"
#include "imageanalyzer.h"
#include "QString"
#include "opencv2/opencv.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "controller.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QString filename = "";
    QString myURL = "https://nominatim.openstreetmap.org/reverse?format=json&lat=50.81638833&lon=4.70138889&zoom=18&addressdetails=1";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setMainWindowTitle();

private slots:

    void on_backToLandingPage_clicked();

    void on_segmentation2dCheckBox_stateChanged(int arg1);

    void on_changeDetectionCheckBox_stateChanged(int arg1);

    void on_segmentation2dBoardButton_pressed();

    void on_segmentation2dBoardButton_released();

    void on_changeDetectionBoardButton_pressed();

    void on_changeDetectionBoardButton_released();

    void on_pushButton2D_pressed();

    void on_pushButton2D_released();

    void on_backToLandingPage2D_clicked();

    void on_start2DImageProcessing_pressed();

    void on_start2DImageProcessing_released();

    void on_choosePath2D_pressed();

    void on_choosePath2D_released();

    void on_resizeSlider_valueChanged(int value);

    void on_chooseFolderChangeDetection2D_pressed();

    void on_chooseFolderChangeDetection2D_released();

    void on_backFromChangeDetection2D_clicked();

    void on_findChangesButton_pressed();

    void on_findChangesButton_released();

    void on_resizeSliderChangeDetection_valueChanged(int value);

    void on_geocodeButton_clicked();

    void geocodingFinished(QNetworkReply*);

private:
    Ui::MainWindow *ui;

    QString checkState(bool a, bool b, bool c);
};
#endif // MAINWINDOW_H
