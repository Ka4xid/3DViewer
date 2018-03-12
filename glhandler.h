#ifndef GLHANDLER_H
#define GLHANDLER_H

#include <QObject>
#include "myglwidget.h"
#include "object_builder.h"
#include "object_class.h"

class GLHandler : public QObject
{
    Q_OBJECT

public:

    GLHandler(QObject *parent = 0);

    QWidget* GetWidgetPointer();

    int BuildSurface(QString mtwFilePath,
                     QString mapFilePath,
                     uint textureScale = 100000);                  // Build surface mesh, based on .mtw height map

    int BuildTrajectory(QString name, QVector<QVector3D> points, float thickness = 5);        // Build trajectory mesh

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    Object_Builder* CreateObject();

public slots:

    void ScaleObject(QString name, QVector4D translation);
    void ScaleObject(int id, QVector4D vector);

    void RotateObject(QString name, QVector4D vector);
    void RotateObject(int id, QVector4D vector);

    void TranslateObject(QString name, QVector4D vector);
    void TranslateObject(int id, QVector4D vector);

    void MoveObjectByDir(QString name,QVector4D vector);
    void MoveObjectByDir(int id, QVector4D vector);

    void DeleteObject(QString name);
    void DeleteObject(int id);

    QVector3D GetObjectPosition(QString name);
    QVector3D GetObjectPosition(int id);

    QVector3D GetObjectRotation(QString name);
    QVector3D GetObjectRotation(int id);

    QVector3D GetObjectScale(QString name);
    QVector3D GetObjectScale(int id);


    void AddObjectToScene(Object_class* newObject);

    void SetCameraPosition(float x, float y, float z);


private:

    MyGLWidget* widget;



    void CreateQuadsForPoints(QVector3D point0, QVector3D point1,
                              QVector3D point2, QVector3D point3,
                              QVector<QVector3D>* pointsCloud, float radius);

};

#endif // GLHANDLER_H
