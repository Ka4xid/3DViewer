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

    // EVENT FUNCTIONS
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void wheelEvent(QWheelEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private:

    void initializeGL();                                // GL related functions
    void resizeGL(int width, int height);               //
    void paintGL();                                     //

    void HandleKeyPresses();
    void SetPerspective(float fov, float Znear, float Zfar, float aspect);  // set perspective view

    // RENDER FUNCTIONS
    void RenderObjectsArray();              // render objects from ObjectsArray
    void RenderAxes (float m);              // render navigation gizmo

public:

    QVector<Object_class*> ObjectsArray;    // Array of Vertex Buffer Objects with parameters

    QVector3D camera_angles;                // Camera rotation
    QVector3D free_camera_pos;              // Free camera position
    QVector3D orb_camera_pos;               // Orbital camera position (center of orbit)

    float distaceFromCamera;                // Orbital camera distance from center

    QMap<int, int> keyMap;                  // Map of currently pressed keys

private:

    QPoint  lastPos;                        // value for tracking last mouse position

    bool wireframeMode;                     // Wireframe drawing mode
    bool freeCamera;

};

#endif // MYGLWIDGET_H
