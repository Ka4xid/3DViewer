#include "glhandler.h"
#include <QDebug>
#include <qmath.h>
#include <QQuaternion>

#include "surface_builder.h"
#include "trajectory_builder.h"
#include "object_builder.h"



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

int GLHandler::BuildTrajectory(QString name, QVector<QVector3D> points, float thickness)
{
    /*Object_class* Trajectory_object = new Object_class;

    Trajectory_object->SetName(name);
    Trajectory_object->SetPolygonType(GL_QUADS);

    QVector<QVector3D> pointsCloud;
    QVector<QVector2D> textureCoords;

    // GENERATE QUADS FOR FIRST TWO POINTS OF TRAJECTORY
    CreateQuadsForPoints(points.at(0), points.at(0),
                         points.at(1), points.at(2),
                         &pointsCloud, thickness);


    // GENERATE QUADS FOR MIDDLE POINTS OF TRAJECTORY
    for (int i=1; i<(points.size()-2); i++) {
        CreateQuadsForPoints(points.at(i-1), points.at(i),
                             points.at(i+1), points.at(i+2),
                             &pointsCloud, thickness);
    }

    // GENERATE QUADS FOR LAST TWO POINTS OF TRAJECTORY
    CreateQuadsForPoints(points.at(points.size()-3), points.at(points.size()-2),
                         points.at(points.size()-1), points.at(points.size()-1),
                         &pointsCloud, thickness);


    // GENERATE TEXTURE COORDS
    foreach (QVector3D point, pointsCloud) {
        point = point;
        textureCoords.append(QVector2D(0.1, 0.25));
    }

    Trajectory_object->SetPointsArray(pointsCloud);
    Trajectory_object->SetTexturesArray(textureCoords);

    AddObjectToScene(Trajectory_object);

    return 1;*/
}

Object_Builder* GLHandler::CreateObject()
{
    Object_Builder* builder = new Object_Builder(widget);
    connect(builder, SIGNAL(ObjectReady(Object_class*)),
            this, SLOT(AddObjectToScene(Object_class*)) );
    return builder;
}

void GLHandler::AddObjectToScene(Object_class* newObject)
{
    newObject->Initialize(widget);

    widget->ObjectsArray.append(newObject);
}

void GLHandler::ScaleObject(QString name, QVector4D scale)
{
    /*int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->name == name) break;
    }

    ScaleObject(id, scale);*/
}
void GLHandler::ScaleObject(int id, QVector4D scale)
{
    /*if ( (id < widget->ObjectsArray.size()) && (id >= 0) ) {
        widget->ObjectsArray[id]->SetScale(scale.toVector3D(), (bool)scale.w() );
    }*/
}

void GLHandler::RotateObject(QString name, QVector4D rotation)
{
    /*int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    RotateObject(id, rotation);*/
}
void GLHandler::RotateObject(int id, QVector4D rotation)
{
    /*if ( (id < widget->ObjectsArray.size()) && (id >= 0) ) {
        widget->ObjectsArray[id]->SetRotation(rotation.toVector3D(), (bool)rotation.w() );
    }*/
}

void GLHandler::TranslateObject(QString name, QVector4D translation)
{
    /*int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    TranslateObject(id, translation);*/
}
void GLHandler::TranslateObject(int id, QVector4D translation)
{
    /*if ( (id < widget->ObjectsArray.size()) && (id >= 0) ) {
        widget->ObjectsArray[id]->SetTranslation(translation.toVector3D(), (bool)translation.w());
    }*/
}

void GLHandler::MoveObjectByDir(QString name, QVector4D vector)
{
    /*int id;
    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    MoveObjectByDir(id, vector);*/
}
void GLHandler::MoveObjectByDir(int id, QVector4D vector)
{
    /*QVector4D rotation;
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

    TranslateObject(id, vector);*/
}

void GLHandler::DeleteObject(QString name)
{
    /*int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }
    DeleteObject(id);*/
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
    /*int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    return GetObjectPosition(id);*/
}
QVector3D GLHandler::GetObjectPosition(int id)
{
    /*QVector3D result;

    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        result = widget->ObjectsArray[id]->GetTranslation();
    }

    return result;*/
}

QVector3D GLHandler::GetObjectRotation(QString name)
{
    /*int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    return GetObjectPosition(id);*/
}
QVector3D GLHandler::GetObjectRotation(int id)
{
    /*QVector3D result;

    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        result = widget->ObjectsArray[id]->GetRotation();
    }

    return result;*/
}

QVector3D GLHandler::GetObjectScale(QString name)
{
    /*int id;

    for (id=0; id < widget->ObjectsArray.size(); id++) {
        if (widget->ObjectsArray.at(id)->GetName() == name) break;
    }

    return GetObjectPosition(id);*/
}
QVector3D GLHandler::GetObjectScale(int id)
{
    /*QVector3D result;

    if ( (id < widget->ObjectsArray.size()) & (id >=0) ) {
        result =  widget->ObjectsArray[id]->GetScale();
    }

    return result;*/
}

void GLHandler::keyPressEvent(QKeyEvent *e)
{
    widget->keyPressEvent(e);
}
void GLHandler::keyReleaseEvent(QKeyEvent *e)
{
    widget->keyReleaseEvent(e);
}

void GLHandler::CreateQuadsForPoints(QVector3D point0, QVector3D point1, QVector3D point2, QVector3D point3,
                                     QVector<QVector3D>* pointsCloud, float radius)
{
    QVector3D relativeVector;
    float angle;
    QQuaternion quat_yaw,
                quat_pitch,
                quat1,
                quat2;

    relativeVector = point2 - point0;

    // 57.2958 - MAGIC NUMBER TO CONVERT FROM RADIANS TO DEGREES
    angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
    quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
    angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
    quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

    quat1 = (quat_yaw * quat_pitch).normalized();

    relativeVector = point3 - point1;

    angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
    quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
    angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
    quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

    quat2 = (quat_yaw * quat_pitch).normalized();



    // TO MAKE MESH SMOOTHER, JUST ADJUST sides_number VALUE
    int sides_number = 8;

    // FIND RADIANS IN ONE QUAD
    float radians = M_PI / (sides_number/2);

    // CREATE QUADS FOR SIDES
    for (int n=0; n<sides_number; n++) {
        pointsCloud->append( point1 + quat1.rotatedVector(QVector3D(cos(radians * n) * radius,
                                                                    sin(radians * n) * radius,
                                                                    0)) );
        pointsCloud->append( point1 + quat1.rotatedVector(QVector3D(cos(radians * (n+1)) * radius,
                                                                    sin(radians * (n+1)) * radius,
                                                                    0)) );
        pointsCloud->append( point2 + quat2.rotatedVector(QVector3D(cos(radians * (n+1)) * radius,
                                                                    sin(radians * (n+1)) * radius,
                                                                    0)) );
        pointsCloud->append( point2 + quat2.rotatedVector(QVector3D(cos(radians * n) * radius,
                                                                    sin(radians * n) * radius,
                                                                    0)) );
    }
}
