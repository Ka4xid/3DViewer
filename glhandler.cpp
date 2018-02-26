#include "glhandler.h"
#include <QDebug>
#include <qmath.h>
#include <QQuaternion>


#define DEBUG_OUTPUT

GLHandler::GLHandler(QObject *parent) : QObject(parent)
{
    widget = new MyGLWidget();

    widget->setGeometry(0, 0, 1280, 720);
}

QWidget* GLHandler::GetWidgetPointer() {
    return widget;
}

void GLHandler::SetCameraPosition(float x, float y, float z)
{
    widget->SetCameraPosition(x, y, z);
}

int GLHandler::BuildSurface(QString mtwFilePath, QString mapFilePath, uint textureScale)
{
    builder = new SurfaceBuilder(this);

    connect(builder, SIGNAL(SetCameraPos(float,float,float)),
            this, SLOT(SetCameraPosition(float,float,float)));
    connect(builder, SIGNAL(SurfaceReady(Object_class*)),
            this, SLOT(PushObjectToArray(Object_class*)) );

    builder->GenerateSurface(mtwFilePath, mapFilePath, textureScale);

    return 1;
}

int GLHandler::BuildTrajectory(QString name, QVector<QVector3D> points)
{
    Object_class* Trajectory_object = new Object_class;

    Trajectory_object->SetName(name);
    Trajectory_object->SetPolygonType(GL_QUADS);

    QVector<QVector3D> pointsCloud;
    QVector<QVector2D> textureCoords;
    QQuaternion quaternion;
    QVector3D rotatedVector;


    quaternion = QQuaternion(QVector4D(0,0,1,90));

    for (int i=0; i<points.size()-1; i++) {

        //quaternion.setVector(points.at(i) - points.at(i+1));

        /*pointsCloud.append(QVector3D(points.at(i).x(),
                                     points.at(i).y(),
                                     points.at(i).z() ));

        pointsCloud.append(QVector3D(points.at(i).x(),
                                     points.at(i).y(),
                                     points.at(i).z()+50 ));

        pointsCloud.append(QVector3D(points.at(i+1).x(),
                                     points.at(i+1).y(),
                                     points.at(i+1).z() ));

        pointsCloud.append(QVector3D(points.at(i+1).x(),
                                     points.at(i+1).y(),
                                     points.at(i+1).z()+50 ));*/
    }


    for(int i=0; i<points.size(); i++) {
        textureCoords.append(QVector2D(0.1,
                                       0.2) );
    }

    Trajectory_object->SetPointsArray(pointsCloud);
    Trajectory_object->SetTexturesArray(textureCoords);

    PushObjectToArray(Trajectory_object);

    return 1;
}


void GLHandler::PushObjectToArray(Object_class* newObject)
{ 
    newObject->Initialize(widget);    

    widget->ObjectsArray.append(newObject);
}

void GLHandler::ScaleObject(QString name, QVector4D scale)
{
    int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    ScaleObject(id, scale);
}
void GLHandler::ScaleObject(int id, QVector4D scale)
{
    if ( (id < widget->ObjectsArray.size()) && (id >= 0) ) {
        widget->ObjectsArray[id]->SetScale(scale.toVector3D(), (bool)scale.w() );
    }
}

void GLHandler::RotateObject(QString name, QVector4D rotation)
{
    int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    RotateObject(id, rotation);
}
void GLHandler::RotateObject(int id, QVector4D rotation)
{
    if ( (id < widget->ObjectsArray.size()) && (id >= 0) ) {
        widget->ObjectsArray[id]->SetRotation(rotation.toVector3D(), (bool)rotation.w() );
    }
}

void GLHandler::TranslateObject(QString name, QVector4D translation)
{
    int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    TranslateObject(id, translation);
}
void GLHandler::TranslateObject(int id, QVector4D translation)
{
    if ( (id < widget->ObjectsArray.size()) && (id >= 0) ) {
        widget->ObjectsArray[id]->SetTranslation(translation.toVector3D(), (bool)translation.w());
    }
}

void GLHandler::MoveObjectByDir(QString name, QVector4D vector)
{
    int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    MoveObjectByDir(id, vector);
}
void GLHandler::MoveObjectByDir(int id, QVector4D vector)
{
    QVector4D rotation;
    QVector3D relativeVector;
    QVector3D currentPosition;

    currentPosition = GetObjectPosition(id);

    if ( vector.w() != 0) {
        relativeVector = (currentPosition + vector.toVector3D()) - currentPosition;
    } else {
        relativeVector = vector.toVector3D() - currentPosition;
    }

    float x = relativeVector.x();
    float y = relativeVector.y();
    float z = relativeVector.z();

    rotation.setZ( atan2(y, x) * 57.2958 );                         // 57.2958 - magic number to
    rotation.setY( (atan2(z, sqrt(x*x + y*y)) * -1) * 57.2958 );  // convert radians to degrees

    RotateObject(id, rotation);

    TranslateObject(id, vector);
}

void GLHandler::DeleteObject(QString name)
{
    int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }
    DeleteObject(id);
}
void GLHandler::DeleteObject(int id)
{
    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        Object_class* obj;
        obj = widget->ObjectsArray[id];
        obj->Delete();
        widget->ObjectsArray.remove(id);
        widget->ObjectsArray.squeeze();
    }
}

QVector3D GLHandler::GetObjectPosition(QString name)
{
    int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    return GetObjectPosition(id);
}
QVector3D GLHandler::GetObjectPosition(int id)
{
    QVector3D result;

    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        result = widget->ObjectsArray[id]->GetTranslation();
    }

    return result;
}

QVector3D GLHandler::GetObjectRotation(QString name)
{
    int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    return GetObjectPosition(id);
}
QVector3D GLHandler::GetObjectRotation(int id)
{
    QVector3D result;

    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        result = widget->ObjectsArray[id]->GetRotation();
    }

    return result;
}

QVector3D GLHandler::GetObjectScale(QString name)
{
    int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    return GetObjectPosition(id);
}
QVector3D GLHandler::GetObjectScale(int id)
{
    QVector3D result;

    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        result =  widget->ObjectsArray[id]->GetScale();
    }

    return result;
}

void GLHandler::keyPressEvent(QKeyEvent *e) {
    widget->keyPressEvent(e);
}
void GLHandler::keyReleaseEvent(QKeyEvent *e) {
    widget->keyReleaseEvent(e);
}
