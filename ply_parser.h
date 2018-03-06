#ifndef PLY_PARSER_H
#define PLY_PARSER_H

#include <QObject>
#include <QFile>
#include "object_class.h"

class ply_parser : public QObject
{
    Q_OBJECT
public:
    ply_parser(Object_class* Object_pointer = 0);
    int ReadObject(QString fileName);

private:

    int OpenFile(QString fileName);
    int ReadData();

private:

    QFile ObjFile;
    QString objName;

    Object_class* Object;


signals:
    void ObjectReady(Object_class* Object);

};

#endif // PLY_PARSER_H
