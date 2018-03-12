#include "ply_parser.h"
#include <QDebug>
#include <QRegExp>

ply_parser::ply_parser(QString fileName)
{
    ObjFile.setFileName(fileName);
}

void ply_parser::ReadData()
{
    QVector<float> verticesData;
    QVector<uint> verticesIndices;

    // Try to open file
    if ( !ObjFile.open(QIODevice::ReadOnly) ) {
        qDebug() << "No such file!";
        return;
    }

    // One regex to rule them all
    QRegExp RegExpData("([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})[ ]([-]?[0-9]*[.]?[0-9]{0,6})");
    QRegExp RegExpIndices("[3][ ]([0-9]*)[ ]([0-9]*)[ ]([0-9]*)");       // Accept only triangulated models

    QTextStream Stream;
    QString line;
    Stream.setDevice(&ObjFile);
    line = Stream.readLine();
    if (line != "ply") {
        qDebug() << "Wrong file format!";
        return;
    }

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

    emit DataReady(verticesData, verticesIndices);
    emit ReadComplete();

    ObjFile.close();
    this->deleteLater();

    return;
}
