#ifndef PLY_PARSER_H
#define PLY_PARSER_H

#include <QObject>
#include <QFile>
#include "object_class.h"

class ply_parser : public QObject
{
    Q_OBJECT
public:
    ply_parser(QString fileName);
    void ReadData();

private:
    QFile ObjFile;

signals:
    void DataReady(QVector<float> p_Data, QVector<uint> p_Indices);
    void ReadComplete();

};

#endif // PLY_PARSER_H
