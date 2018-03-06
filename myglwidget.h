#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QObject>
#include <QGL>
#include <QGLBuffer>
#include <QGLShader>
#include <QPoint>
#include <QGLFunctions>
#include "3D_viewer/object_class.h"



class MyGLWidget : public QGLWidget, private QGLFunctions
{

public:
    MyGLWidget();

    void keyPressEvent(QKeyEvent *e);                               //
    void keyReleaseEvent(QKeyEvent *e);                             //
    void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent *e);                        //
    void SetCameraPosition(float x, float y, float z);


private:

    void handleKeys();
    void initializeGL();                                // GL related functions
    void resizeGL(int width, int height);               //
    void paintGL();                                     //
    void SetPerspective(float fov, float Znear, float Zfar, float aspect);  // set perspective view

    void RenderObjectsArray();                          // render objects from ObjectsArray

    void RenderOcta(float m);           // render test octahedron
    void RenderGizmo(float m);          // render navigation gizmo
    void RenderCrosshair(float m);      // render basic crosshair

public:

    QVector<Object_class*> ObjectsArray;     // Array of Vertex Buffer Objects with parameters


private:

    QVector3D free_camera_pos;
    QVector3D orb_camera_pos;
    QVector3D camera_angles;

    QPoint  lastPos;                // value for tracking last mouse position

    bool wireframeMode;             // Wireframe drawing mode
    float colorModel;               // Height map coloring mode
    bool freeCamera;

    float distaceFromCamera;

    QMap<int, int> keyMap;          // Used for handling simlutaneous key presses
};

#endif // MYGLWIDGET_H
