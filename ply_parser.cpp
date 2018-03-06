#include "ply_parser.h"
#include <QDebug>
#include <QRegExp>

ply_parser::ply_parser(Object_class *Object_pointer) : QObject(Object_pointer)
{
    Object = Object_pointer;
}

int ply_parser::ReadObject(QString fileName)
{
    if (OpenFile(fileName)) {
        if ( ReadData() ) {
            qDebug() << "PLY read";
            return 1;
        }
    }
    qDebug() << "Object file read failed!";
    return -1;
}

int ply_parser::OpenFile(QString fileName)
{
    ObjFile.setFileName(fileName);
    if ( ObjFile.open(QIODevice::ReadOnly) ) {
        return 1;
    }
    return -1;
}

int ply_parser::ReadData()
{
    QVector<float> verticesData;
    QVector<uint> verticesIndices;

    // I hate regexps
    QRegExp RegExpData("([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})");
    QRegExp RegExpIndices("[3][ ]([0-9]*)[ ]([0-9]*)[ ]([0-9]*)");       // Accept only triangulated models

    QTextStream Stream;
    QString line;

    Stream.setDevice(&ObjFile);

    while( (!(Stream.atEnd()))) {
        line = Stream.readLine();

        if (RegExpData.exactMatch(line)) {
            for (int n=1; n<=8; n++) {
                verticesData.append( RegExpData.cap(n).toFloat() );
            }
        }

        if (RegExpIndices.exactMatch(line)) {
            for (int n=1; n<=3; n++) {
                verticesIndices.append( RegExpIndices.cap(n).toUInt() );
            }
        }

    }

    if (Object == 0) {
        Object = new Object_class;
        Object->SetName(objName);
    }

    Object->SetPointsVector(verticesData, verticesIndices);

    emit ObjectReady(Object);

    ObjFile.close();

    this->deleteLater();

    return 1;
}
