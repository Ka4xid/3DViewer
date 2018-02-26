#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <QObject>
#include <QFile>
#include <3D_viewer/object_class.h>


class obj_parser : public QObject
{
    Q_OBJECT
public:

    obj_parser(Object_class* Object_pointer = 0);
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

#endif // OBJ_PARSER_H
