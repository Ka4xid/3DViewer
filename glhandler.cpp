#include "glhandler.h"
#include <QDebug>
#include <qmath.h>
#include <QQuaternion>

#include "surface_builder.h"



#define DEBUG_OUTPUT

GLHandler::GLHandler(QObject *parent) : QObject(parent)
{
    qRegisterMetaType< QVector<float> >("QVector<float>");
    qRegisterMetaType< QVector<uint> >("QVector<uint>");

    widget = new MyGLWidget();
    widget->setGeometry(0, 0, 1280, 720);
}

QWidget* GLHandler::GetWidgetPointer() {
    return widget;
}

void GLHandler::SetCameraPosition(float x, float y, float z)
{
    widget->free_camera_pos = QVector3D(x,y,z);
    widget->orb_camera_pos = QVector3D(x,y,z);
}

int GLHandler::BuildSurface(QString mtwFilePath, QString mapFilePath, uint textureScale)
{
    Surface_Builder* builder = new Surface_Builder(this);

    connect(builder, SIGNAL(SetCameraPos(float,float,float)),
            this, SLOT(SetCameraPosition(float,float,float)));
    connect(builder, SIGNAL(SurfaceReady(Object_class*)),
            this, SLOT(AddObjectToScene(Object_class*)) );

    builder->GenerateSurface(mtwFilePath, mapFilePath, textureScale);

    return 1;
}

trajectory_builder* GLHandler::CreateTrajectory()
{
    trajectory_builder* builder = new trajectory_builder();
    connect(builder, SIGNAL(ObjectReady(Object_class*)),
            this, SLOT(AddObjectToScene(Object_class*)));
    return builder;
}

Object_Builder* GLHandler::CreateObject()
{
    Object_Builder* builder = new Object_Builder();
    connect(builder, SIGNAL(ObjectReady(Object_class*)),
            this, SLOT(AddObjectToScene(Object_class*)) );
    return builder;
}

void GLHandler::AddObjectToScene(Object_class* newObject)
{
    newObject->Initialize(widget);

    widget->ObjectsArray.append(newObject);
}

// TRANSFORM OPERATIONS
void GLHandler::ScaleObject(QString name, QVector3D scale)
{
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            object->scale = scale;
            break;
        }
    }
}
void GLHandler::RotateObject(QString name, QVector3D rotation)
{
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            object->rotation = rotation;
            break;
        }
    }
}
void GLHandler::TranslateObject(QString name, QVector3D translation)
{
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            object->translation = translation;
            break;
        }
    }
}
QVector3D GLHandler::GetObjectTranslation(QString name)
{
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            return object->translation;
        }
    }
}
QVector3D GLHandler::GetObjectRotation(QString name)
{
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            return object->rotation;
        }
    }
}
QVector3D GLHandler::GetObjectScale(QString name)
{
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            return object->scale;
        }
    }
}

void GLHandler::MoveObjectByDir(QString name, QVector3D vector)
{
    QVector3D rotation;
    QVector3D relativeVector;
    QVector3D currentPosition;

    currentPosition = GetObjectTranslation(name);
    relativeVector = (vector - currentPosition).normalized();

    float x = relativeVector.x();
    float y = relativeVector.y();
    float z = relativeVector.z();

    rotation.setZ( atan2(y, x) * 57.2958 );                         // 57.2958 - magic number to
    rotation.setY( (atan2(z, sqrt(x*x + y*y)) * -1) * 57.2958 );    // convert radians to degrees

    RotateObject(name, rotation);

    TranslateObject(name, vector);
}


void GLHandler::DeleteObject(QString name)
{
    uint id=0;
    foreach (Object_class* object, widget->ObjectsArray) {
        if (object->name == name) {
            object->Delete();
            widget->ObjectsArray.remove(id);
            widget->ObjectsArray.squeeze();
            break;
        }
        id++;
    }
}


void GLHandler::keyPressEvent(QKeyEvent *e)
{
    widget->keyPressEvent(e);
}
void GLHandler::keyReleaseEvent(QKeyEvent *e)
{
    widget->keyReleaseEvent(e);
}

