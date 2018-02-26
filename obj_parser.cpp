#include "obj_parser.h"
#include <QDebug>
#include <QRegExp>



obj_parser::obj_parser(Object_class* Object_pointer) : QObject()
{
    Object = Object_pointer;
}

int obj_parser::ReadObject(QString fileName)
{
    if ( OpenFile(fileName) ) {
        qDebug() << "File Opened";
        if ( ReadData() ) {
            //all done
            qDebug() << "Data read";

            return 1;
        }
        qDebug() << "Data not read";
    }

    return -1;
}

int obj_parser::OpenFile(QString fileName)
{
    ObjFile.setFileName(fileName);
    if ( ObjFile.open(QIODevice::ReadOnly) ) {
        return 1;
    }
    return -1;
}

int obj_parser::ReadData()
{
    QVector<QVector3D> pointVectors_t;
    QVector<QVector2D> textureVectors_t;
    QVector<QVector3D> normalVectors_t;

    QVector<QVector3D> pointVectors;
    QVector<QVector2D> textureVectors;
    QVector<QVector3D> normalVectors;

    QRegExp RegExpName("^o[ ](.*)");
    QRegExp RegExpVert("^v[ ](.*)[ ](.*)[ ](.*)");
    QRegExp RegExpText("^vt[ ](.*)[ ](.*)");
    QRegExp RegExpNorm("^vn[ ](.*)[ ](.*)[ ](.*)");
    QRegExp RegExpFace("^f[ ](.*)/(.*)/(.*)[ ](.*)/(.*)/(.*)[ ](.*)/(.*)/(.*)");


    QTextStream Stream;
    QString line;

    Stream.setDevice(&ObjFile);


    while( (!(Stream.atEnd()))) {
        line = Stream.readLine();

        if (RegExpVert.exactMatch(line)) {
            /*qDebug() << "V " << RegExpVert.cap(1).trimmed()
                               << RegExpVert.cap(2).trimmed()
                               << RegExpVert.cap(3).trimmed();*/

            pointVectors_t.append(QVector3D(RegExpVert.cap(1).toFloat(),
                                            RegExpVert.cap(2).toFloat()*-1,     // BECAUSE OF LEFT-HANDED COORDINATES
                                            RegExpVert.cap(3).toFloat() ) );    // OR RIGHT, I DONT EVEN KNOW ANYMORE
            continue;
        }

        if (RegExpText.exactMatch(line)) {
            /*qDebug() << "T " << RegExpText.cap(1).trimmed()
                               << RegExpText.cap(2).trimmed();*/

            textureVectors_t.append(QVector2D(RegExpText.cap(1).toFloat(),
                                              RegExpText.cap(2).toFloat() ) );
            continue;
        }

        if (RegExpNorm.exactMatch(line)) {
            /*qDebug() << "N " << RegExpFace.cap(1).trimmed()
                               << RegExpFace.cap(2).trimmed()
                               << RegExpFace.cap(3).trimmed();*/
            normalVectors_t.append(QVector3D(RegExpNorm.cap(1).toFloat(),
                                             RegExpNorm.cap(2).toFloat(),
                                             RegExpNorm.cap(3).toFloat() ) );
            continue;
        }

        if (RegExpFace.exactMatch(line)) {
            /*qDebug() << RegExpFace.cap(1).toInt()-1
                       << RegExpFace.cap(2).toInt()-1
                       << RegExpFace.cap(3).toInt()-1
                       << RegExpFace.cap(4).toInt()-1
                       << RegExpFace.cap(5).toInt()-1
                       << RegExpFace.cap(6).toInt()-1
                       << RegExpFace.cap(7).toInt()-1
                       << RegExpFace.cap(8).toInt()-1
                       << RegExpFace.cap(9).toInt()-1;*/

            pointVectors.append(   pointVectors_t.at(  RegExpFace.cap(1).toInt()-1) );
            textureVectors.append( textureVectors_t.at(RegExpFace.cap(2).toInt()-1) );
            normalVectors.append(  normalVectors_t.at( RegExpFace.cap(3).toInt()-1) );
            pointVectors.append(   pointVectors_t.at(  RegExpFace.cap(4).toInt()-1) );
            textureVectors.append( textureVectors_t.at(RegExpFace.cap(5).toInt()-1) );
            normalVectors.append(  normalVectors_t.at( RegExpFace.cap(6).toInt()-1) );
            pointVectors.append(   pointVectors_t.at(  RegExpFace.cap(7).toInt()-1) );
            textureVectors.append( textureVectors_t.at(RegExpFace.cap(8).toInt()-1) );
            normalVectors.append(  normalVectors_t.at( RegExpFace.cap(9).toInt()-1) );

            continue;
        }

        if (RegExpName.exactMatch(line)) {
            //qDebug() << RegExpName.cap(1).trimmed();
            objName = RegExpName.cap(1).trimmed();
            continue;
        }

    }

    if (Object == 0) {
        Object = new Object_class;
        Object->SetName(objName);
    }

    Object->SetPointsArray(pointVectors);
    Object->SetTexturesArray(textureVectors);
    Object->SetNormalsArray(normalVectors);

    emit ObjectReady(Object);

    ObjFile.close();

    this->deleteLater();

    return 1;
}

