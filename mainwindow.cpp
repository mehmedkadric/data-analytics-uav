#include "mainwindow.h"
#include "ui_mainwindow.h"



#define LANDINGPAGE 0
#define LANDINGPAGE2D 1
#define BOARD2D 2
#define CHANGEDETETION2D 3


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_backToLandingPage_clicked()
{
    ui->StackedWidgets->setCurrentIndex(LANDINGPAGE);
}


void MainWindow::on_segmentation2dCheckBox_stateChanged(int arg1)
{
    ui->infoLabel->setText(checkState(ui->changeDetectionCheckBox->isChecked(), arg1, ui->watershedCheckBox->isChecked()));
}

void MainWindow::on_changeDetectionCheckBox_stateChanged(int arg1)
{
    ui->infoLabel->setText(checkState(arg1, ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()));
}


QString MainWindow::checkState(bool a, bool b, bool c){
    if(a && b) {
        return "Segmentation 2D and Contour detection";
    } else if (a && !b){
        return "Contour detection";
    } else if (!a && b){
        return "Segmentation 2D";
    } else if (c && !a && !b) {
        return "Watershed segmentation";
    } else {
        return "Raw image preview";
    }
}


void MainWindow::on_segmentation2dBoardButton_pressed()
{
    ui->segmentation2dBoardButton->setStyleSheet("QPushButton{ background-color: green; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                 "text-align: center;   text-decoration: none;   font-size: 16px; }");
}


void MainWindow::on_segmentation2dBoardButton_released()
{
    ui->segmentation2dBoardButton->setStyleSheet("QPushButton{   background-color: #4CAF50; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                 "text-align: center;   text-decoration: none;   font-size: 16px; }");
    ui->infoLabel->setText(checkState(ui->changeDetectionCheckBox->isChecked(), ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()));
    QString label = "Resize image constant: " + QString::number(ui->resizeSlider->value());
    ui->resizeLabel->setText(label);
    ui->StackedWidgets->setCurrentIndex(BOARD2D);
}


void MainWindow::on_changeDetectionBoardButton_pressed()
{
    ui->changeDetectionBoardButton->setStyleSheet("QPushButton{ background-color: green; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
}


void MainWindow::on_changeDetectionBoardButton_released()
{
    ui->changeDetectionBoardButton->setStyleSheet("QPushButton{   background-color: #4CAF50; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");

    ui->pathLabelChangeDetection->setText(filename);
    QString label2 = "Resize image constant: " + QString::number(ui->resizeSliderChangeDetection->value());
    ui->resizeLabelChangeDetection->setText(label2);
    ui->StackedWidgets->setCurrentIndex(CHANGEDETETION2D);
}


void MainWindow::on_pushButton2D_pressed()
{
    ui->pushButton2D->setStyleSheet("QPushButton{ background-color: green; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
}


void MainWindow::on_pushButton2D_released()
{
    ui->pushButton2D->setStyleSheet("QPushButton{   background-color: #4CAF50; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
    ui->StackedWidgets->setCurrentIndex(LANDINGPAGE2D);
}


void MainWindow::on_backToLandingPage2D_clicked()
{
    ui->StackedWidgets->setCurrentIndex(LANDINGPAGE2D);
}


void MainWindow::on_start2DImageProcessing_pressed()
{
    ui->start2DImageProcessing->setStyleSheet("QPushButton{ background-color: green; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
}


void MainWindow::on_start2DImageProcessing_released()
{
    ui->start2DImageProcessing->setStyleSheet("QPushButton{   background-color: #4CAF50; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
    ui->resizeSlider->setEnabled(0);
    if(this->filename == "") {
        Controller controller;
        ui->pathLabel->setText("/Users/mehmed/Desktop/uavData");
        controller.handleInput("/Users/mehmed/Desktop/uavData", checkState(ui->changeDetectionCheckBox->isChecked(), ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()), ui->resizeSlider->value());
    }
    else {
        Controller controller;
        controller.handleInput(filename, checkState(ui->changeDetectionCheckBox->isChecked(), ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()), ui->resizeSlider->value());
    }
    ui->resizeSlider->setEnabled(1);
}


void MainWindow::on_choosePath2D_pressed()
{
    ui->choosePath2D->setStyleSheet("QPushButton{   background-color: gray; border: 1px solid #ccc; padding: 6px 12px; }");
}


void MainWindow::on_choosePath2D_released()
{
    ui->choosePath2D->setStyleSheet("QPushButton{ border: 1px solid #ccc; padding: 6px 12px; }");
    QString filename = QFileDialog::getExistingDirectory(this, "Get Any File");
    this->filename = filename;
    if(this->filename.size() > 50) filename = filename.mid(0, 47) + "...";
    ui->pathLabel->setText(filename);
}


void MainWindow::setMainWindowTitle(){
    this->setWindowTitle("Semantic Classification and Object Segmentation");
}


void MainWindow::on_resizeSlider_valueChanged(int value)
{
    QString label = "Resize image constant: " + QString::number(value);
    ui->resizeLabel->setText(label);
}


void MainWindow::on_chooseFolderChangeDetection2D_pressed()
{
    ui->chooseFolderChangeDetection2D->setStyleSheet("QPushButton{   background-color: gray; border: 1px solid #ccc; padding: 6px 12px; }");
}



void MainWindow::on_chooseFolderChangeDetection2D_released()
{
    ui->chooseFolderChangeDetection2D->setStyleSheet("QPushButton{ border: 1px solid #ccc; padding: 6px 12px; }");
    QString filename = QFileDialog::getExistingDirectory(this, "Get Any File");
    this->filename = filename;
    if(this->filename.size() > 50) filename = filename.mid(0, 47) + "...";
    ui->pathLabelChangeDetection->setText(filename);

}


void MainWindow::on_backFromChangeDetection2D_clicked()
{
    ui->StackedWidgets->setCurrentIndex(LANDINGPAGE2D);
}


void MainWindow::on_findChangesButton_pressed()
{
    ui->findChangesButton->setStyleSheet("QPushButton{   background-color: #4CAF50; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
}


void MainWindow::on_findChangesButton_released()
{
    ui->findChangesButton->setStyleSheet("QPushButton{   background-color: #4CAF50; border: 1px solid #ccc; color: white;   padding: 15px 32px;"
                                                  "text-align: center;   text-decoration: none;   font-size: 16px; }");
    ui->resizeSliderChangeDetection->setEnabled(0);
    if(this->filename == "") {
        filename = "/Users/mehmed/Desktop/uavData";
        ui->pathLabelChangeDetection->setText(filename);
        Controller changeDetectionController;
        changeDetectionController.handleChangeDetection(filename, ui->resizeSliderChangeDetection->value());
    }
    else {
        Controller controller;
        controller.handleChangeDetection(filename, ui->resizeSliderChangeDetection->value());

    }
    ui->resizeSliderChangeDetection->setEnabled(1);
}

void MainWindow::on_resizeSliderChangeDetection_valueChanged(int value)
{
    QString label = "Resize image constant: " + QString::number(ui->resizeSliderChangeDetection->value());
    ui->resizeLabelChangeDetection->setText(label);
}

void MainWindow::on_geocodeButton_clicked()
{
    QNetworkAccessManager *man = new QNetworkAccessManager(this);
    connect(man, &QNetworkAccessManager::finished, this, &MainWindow::geocodingFinished);
    const QUrl url = QUrl(myURL);
    QNetworkRequest request(url);
    man->get(request);
}

void MainWindow::geocodingFinished(QNetworkReply *reply) {
    QString json = reply->readAll();
    ui->infoLabel->setText(json);
}
