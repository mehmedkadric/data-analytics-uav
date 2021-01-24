#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>
#include "jsonparser.h"
#include "geocoder.h"


#define LANDINGPAGE 0
#define LANDINGPAGE2D 1
#define BOARD2D 2
#define VIEW 3
#define CHANGEDETETION2D 4



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , controller()
{
    ui->setupUi(this);
    connect(ui->previousImage, SIGNAL(released()), &controller, SLOT(previousImage()));
    connect(ui->nextImage, SIGNAL(released()), &controller, SLOT(nextImage()));
    connect(&controller, SIGNAL(sendImage(QPixmap)), this, SLOT(previewImage(QPixmap)));
    connect(&controller, SIGNAL(updateDistance(const QString)), ui->distanceLabel, SLOT(setText(const QString)));
    connect(&controller, SIGNAL(updateImageDescription(const QString)), ui->imageDetails, SLOT(setText(const QString)));

}

void MainWindow::previewImage(QPixmap imgPix) {
    int w = ui->viewLabel->width();
    int h = ui->viewLabel->height();
    ui->viewLabel->setPixmap(imgPix.scaled(w,h,Qt::KeepAspectRatio));
    QString title;
    title = this->controller.getCurrentImagePath();
    title = title.split("/")[title.split('/').size()-1];
    title += "(" + QString::number(controller.getCurrentImageIndex() + 1) + "/" + QString::number(controller.getImages().size()) + ")";
    ui->imageTitle->setText(title);
    ui->geocodingData->setText("");
    ui->coordinatesLabel->setText("");

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
        this->controller.setTask("Segmentation 2D and Contour detection");
        return "Segmentation 2D and Contour detection";
    } else if (a && !b){
        this->controller.setTask("Contour detection");
        return "Contour detection";
    } else if (!a && b){
        this->controller.setTask("Segmentation 2D");
        return "Segmentation 2D";
    } else if (c && !a && !b) {
        this->controller.setTask("Watershed segmentation");
        return "Watershed segmentation";
    } else {
        this->controller.setTask("Raw image preview");
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
        ui->pathLabel->setText("/Users/mehmed/Desktop/uavData");
        //controller.handleInput("/Users/mehmed/Desktop/uavData", checkState(ui->changeDetectionCheckBox->isChecked(), ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()), ui->resizeSlider->value());
    }
    else {        
        //controller.handleInput(filename, checkState(ui->changeDetectionCheckBox->isChecked(), ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()), ui->resizeSlider->value());
    }
    controller.setTask(checkState(ui->changeDetectionCheckBox->isChecked(), ui->segmentation2dCheckBox->isChecked(), ui->watershedCheckBox->isChecked()));
    ui->StackedWidgets->setCurrentIndex(VIEW);
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
    //if(this->filename.size() > 50) filename = filename.mid(0, 47) + "...";
    ui->pathLabel->setText(filename);
    controller.loadImages(filename);
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
        controller.handleChangeDetection(filename, ui->resizeSliderChangeDetection->value());
    }
    else {
        controller.handleChangeDetection(filename, ui->resizeSliderChangeDetection->value());

    }
    ui->resizeSliderChangeDetection->setEnabled(1);
}

void MainWindow::on_resizeSliderChangeDetection_valueChanged(int value)
{
    QString label = "Resize image constant: " + QString::number(ui->resizeSliderChangeDetection->value());
    ui->resizeLabelChangeDetection->setText(label);
}


void MainWindow::on_BackFromView_clicked()
{
    ui->StackedWidgets->setCurrentIndex(BOARD2D);
}

void MainWindow::on_reverseGeocode_released()
{
    Geocoder geocoder;
    bool geocode = false;
    if(geocode && controller.getImages().size() > 0){
        QString url = "https://nominatim.openstreetmap.org/reverse?format=json&lat="
                + QString::number(geocoder.getLatitude(controller.getCurrentImagePath())) + "&lon="
                + QString::number(geocoder.getLongitude(controller.getCurrentImagePath())) + "&zoom=18&addressdetails=1";
        QNetworkAccessManager *man = new QNetworkAccessManager(this);

        connect(man, &QNetworkAccessManager::finished, this, &MainWindow::geocodingFinished);
        QNetworkRequest request(url);
        man->get(request);
    } else if(controller.getImages().size() > 0) {
        QString jsonData = "{\"place_id\": 208164950, \"licence\": \"Data © OpenStreetMap contributors, ODbL 1.0. https://osm.org/copyright\", \"osm_type\": \"way\", \"osm_id\": 557730236, \"lat\": \"50.813414800000004\", \"lon\": \"4.700720669769199\", \"display_name\": \"Bosreservaat Grote Konijnenpijp, Prosperdreef, Haasrode, Oud-Heverlee, Leuven, Flemish Brabant, Flanders, 3053, Belgium\", \"address\": {\"leisure\": \"Bosreservaat Grote Konijnenpijp\", \"road\": \"Prosperdreef\", \"village\": \"Haasrode\", \"town\": \"Oud-Heverlee\", \"county\": \"Leuven\", \"state\": \"Flemish Brabant\", \"region\": \"Flanders\", \"postcode\": \"3053\", \"country\": \"Belgium\", \"country_code\": \"be\"}, \"boundingbox\": [\"50.8106238\", \"50.8159908\", \"4.6962288\", \"4.7056092\"] }";
        JsonParser parser;
        QMap<QString, QString> address = parser.getAddress(jsonData);
        QString msg = "Nearest road: " + address.take("road").toUpper() + "\nTown: " + address.take("town").toUpper() + "\nCountry: " + address.take("country").toUpper();
        msg += "\nDistance between sent and received coordinates: " + QString::number(static_cast<int>(geocoder.getDistanceFromLatLongInKm(geocoder.getLatitude(controller.getCurrentImagePath()),
                                                                                                                                           geocoder.getLongitude(controller.getCurrentImagePath()),
                                                                                                                                           parser.getObtainedLatitude(jsonData).toDouble(),
                                                                                                                                           parser.getObtainedLongitude(jsonData).toDouble()) * 1000)) + " [m]";
        ui->geocodingData->setText(msg);
        ui->coordinatesLabel->setText("POINT(" + QString::number(geocoder.getLatitude(controller.getCurrentImagePath())) + ", " + QString::number(geocoder.getLongitude(controller.getCurrentImagePath())) + ")");
    } else {
        ui->geocodingData->setText("Unable to resolve!");
    }
}

void MainWindow::geocodingFinished(QNetworkReply *reply) {
    QString jsonData = reply->readAll();
    qDebug() << jsonData;
    Geocoder geocoder;
    JsonParser jsonParser;
    QMap<QString, QString> address = jsonParser.getAddress(jsonData);
    QString msg = "Nearest road: " + address.take("road").toUpper() + "\nTown: " + address.take("town").toUpper() + "\nCountry: " + address.take("country").toUpper();
    msg += "\nDistance between sent and received coordinates: " + QString::number(static_cast<int>(geocoder.getDistanceFromLatLongInKm(geocoder.getLatitude(controller.getCurrentImagePath()),
                                                                                                                                       geocoder.getLongitude(controller.getCurrentImagePath()),                                                                                                  jsonParser.getObtainedLatitude(jsonData).toDouble(),
                                                                                                                                       jsonParser.getObtainedLongitude(jsonData).toDouble()) * 1000)) + " [m]";
    ui->geocodingData->setText(msg);

}
