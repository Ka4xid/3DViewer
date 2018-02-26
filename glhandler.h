#ifndef GLHANDLER_H
#define GLHANDLER_H

#include <QObject>
#include "myglwidget.h"
#include "surfacebuilder.h"
#include "3D_viewer/object_class.h"

class GLHandler : public QObject
{
    Q_OBJECT

public:
    GLHandler(QObject *parent = 0);

    QWidget* GetWidgetPointer();

    int BuildSurface(QString mtwFilePath, QString mapFilePath, uint textureScale = 100000);                  // Build surface mesh, based on height map

    int BuildTrajectory(QString name, QVector<QVector3D> points);        // Build trajectory mesh

    void keyPressEvent(QKeyEvent *e);                       //
    void keyReleaseEvent(QKeyEvent *e);                     //

public slots:

    void ScaleObject(QString name, QVector4D translation);       // find object with name and pass its ID & values to next func
    void ScaleObject(int id, QVector4D vector);             // change scale of object with ID

    void RotateObject(QString name, QVector4D vector);       // find object with name and pass its ID & values to next func
    void RotateObject(int id, QVector4D vector);             // change rotation of object with ID

    void TranslateObject(QString name, QVector4D vector);     // find object with name and pass its ID & values to next func
    void TranslateObject(int id, QVector4D vector);           // change translation of object with ID

    void MoveObjectByDir(QString name,QVector4D vector);       // find object by name and pass its ID & values to next func
    void MoveObjectByDir(int id, QVector4D vector);             // translate and rotate object with ID by input vector

    void DeleteObject(QString name);                        // find object with name and pass its id to next func
    void DeleteObject(int id);                              // delete object with ID

    QVector3D GetObjectPosition(QString name);       // find object with name and return its position (translation in world-coords)
    QVector3D GetObjectPosition(int id);             //

    QVector3D GetObjectRotation(QString name);       // find object with name and return its rotation
    QVector3D GetObjectRotation(int id);             //

    QVector3D GetObjectScale(QString name);          // find object and return its scale
    QVector3D GetObjectScale(int id);                //


    void PushObjectToArray(Object_class* newObject);           // Add object with specified parameters to scene

    void SetCameraPosition(float x, float y, float z);


private:
    MyGLWidget* widget;
    SurfaceBuilder *builder;

    void CreateSphereAtPoint(QVector<float> *pointsCloud, float x, float y, float z);       // generate octahedron at specified points


};

#endif // GLHANDLER_H
