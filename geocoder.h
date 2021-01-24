#ifndef GEOCODER_H
#define GEOCODER_H

#include <QString>
#include "exif.h"
#include <QNetworkAccessManager>

class Geocoder : public QObject
{
    Q_OBJECT
public:
    Geocoder();
    double getLatitude(QString imagePath);
    double getLongitude(QString imagePath);
    QString reverseGeocode(double latitude, double longitude);
    double getDistanceFromLatLongInKm(double lat1, double long1, double lat2, double long2);
    void geocodingFinished(QNetworkReply *reply);
    bool checkIfImageIsGeocoded(QString imgPath);

private:
    double deg2rad(double deg);
    easyexif::EXIFInfo getExifInfo(QString imagePath);
};

#endif // GEOCODER_H
