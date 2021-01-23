#include "geocoder.h"
#include "exif.h"
#include <math.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include "jsonparser.h"

#define PI 3.14159265359

Geocoder::Geocoder()
{

}

double Geocoder::getLatitude(QString imagePath) {
    try {
        easyexif::EXIFInfo result = getExifInfo(imagePath);
        return result.GeoLocation.Latitude;
    } catch ( ... ) {
        return -1;
    }
}

double Geocoder::getLongitude(QString imagePath) {
    try {
        easyexif::EXIFInfo result = getExifInfo(imagePath);
        return result.GeoLocation.Longitude;
    } catch ( ... ) {
        return -1;
    }
}

QString Geocoder::reverseGeocode(double latitude, double longitude) {
    QString url = "https://nominatim.openstreetmap.org/reverse?format=json&lat="
            + QString::number(latitude) + "&lon="
            + QString::number(longitude) + "&zoom=18&addressdetails=1";
    qDebug() << url;
    QNetworkAccessManager *man = new QNetworkAccessManager();
    connect(man, &QNetworkAccessManager::finished, this, &Geocoder::geocodingFinished);
    QNetworkRequest request(url);
    man->get(request);

    return url;
}

bool Geocoder::checkIfImageIsGeocoded(QString imgPath) {
    return true;
}

void Geocoder::geocodingFinished(QNetworkReply *reply) {
    QString json = reply->readAll();
    JsonParser jsonParser;
    QMap<QString, QString> address = jsonParser.getAddress(json);
    //TO BE CONTINUED...
}


double Geocoder::getDistanceFromLatLongInKm(double lat1, double lon1, double lat2, double lon2) {
    int R = 6371; //radius of the earth in km

    /* Haversine formula */
    double dLat = deg2rad(lat2 - lat1);
    double dLon = deg2rad(lon2 - lon1);
    double a = sin(dLat/2) * sin(dLat/2) + cos(deg2rad(lat1)) * cos(deg2rad(lat2)) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    double d = R * c;

    return d;
}

double Geocoder::deg2rad(double deg) {
    return deg * (PI/180.0);
}

easyexif::EXIFInfo Geocoder::getExifInfo(QString imagePath) {
    // Read the JPEG file into a buffer
      FILE *fp = fopen(imagePath.toStdString().c_str(), "rb");
      if (!fp) {
        throw "Can't open file.\n";
      }
      fseek(fp, 0, SEEK_END);
      unsigned long fsize = ftell(fp);
      rewind(fp);
      unsigned char *buf = new unsigned char[fsize];
      if (fread(buf, 1, fsize, fp) != fsize) {
        delete[] buf;
        throw "Can't read file.\n";
      }

      fclose(fp);

      // Parse EXIF
      easyexif::EXIFInfo result;
      int code = result.parseFrom(buf, fsize);
      delete[] buf;

      if (code) {
        throw "Error parsing EXIF: code %d\n" + code;
      }
      return result;
}
