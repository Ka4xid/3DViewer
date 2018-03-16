#ifndef GLHANDLER_H
#define GLHANDLER_H

#include <QObject>
#include "myglwidget.h"
#include "object_class.h"
#include "trajectory_builder.h"
#include "object_builder.h"

class GLHandler : public QObject
{
    Q_OBJECT

public:

    GLHandler(QObject *parent = 0);

    QWidget* GetWidgetPointer();

    int BuildSurface(QString mtwFilePath,
                     QString mapFilePath,
                     uint textureScale = 100000);                  // Build surface mesh, based on .mtw height map


    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);


    trajectory_builder* CreateTrajectory();
    Object_Builder* CreateObject();


public slots:

    void ScaleObject(QString name, QVector3D translation);
    void RotateObject(QString name, QVector3D vector);
    void TranslateObject(QString name, QVector3D vector);
    QVector3D GetObjectTranslation(QString name);
    QVector3D GetObjectRotation(QString name);
    QVector3D GetObjectScale(QString name);

    void MoveObjectByDir(QString name,QVector3D vector);

    void DeleteObject(QString name);

    void AddObjectToScene(Object_class* newObject);

    void SetCameraPosition(QVector3D newPosition);


private:

    MyGLWidget* widget;



    void CreateQuadsForPoints(QVector3D point0, QVector3D point1,
                              QVector3D point2, QVector3D point3,
                              QVector<QVector3D>* pointsCloud, float radius);

};

#endif // GLHANDLER_H
