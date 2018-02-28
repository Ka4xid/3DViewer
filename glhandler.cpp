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


    // GENERATING QUADS
    QQuaternion quat_yaw,
                quat_pitch,
                quat_summ_first,
                quat_summ_second;

    float angle;
    QVector3D firstPoint;
    QVector3D secondPoint;
    QVector3D relativeVector;

    float size = 3;


    // GENERATE QUADS FOR FIRST TWO POINTS OF TRAJECTORY
    {
        firstPoint = points.at(0);
        secondPoint = points.at(1);

        relativeVector = points.at(1) - points.at(0);

        angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
        quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
        angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
        quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

        quat_summ_first = (quat_yaw * quat_pitch).normalized();

        relativeVector = points.at(2) - points.at(0);

        angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
        quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
        angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
        quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

        quat_summ_second = quat_yaw * quat_pitch;

        // CREATING QUADS
        // QUAD 1
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, -size, 0)) );

        // QUAD 2
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, -size, 0)) );

        // QUAD 3
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, size, 0)) );

        // QUAD 4
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, -size, 0)) );
    }

    // GENERATE QUADS FOR MIDDLE POINTS OF TRAJECTORY
    for (int i=1; i<(points.size()-2); i++) {

        firstPoint = points.at(i);
        secondPoint = points.at(i+1);

        // CALCULATING ROTATIONS FOR FIRST POINT IN CYCLE

        relativeVector = points.at(i+1) - points.at(i-1);

        angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
        quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
        angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
        quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

        quat_summ_first = (quat_yaw * quat_pitch).normalized();

        // CALCULATING ROTATIONS FOR SECOND POINT
        relativeVector = points.at(i+2) - points.at(i);

        angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
        quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
        angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
        quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

        quat_summ_second = quat_yaw * quat_pitch;


        // CREATING QUADS
        // QUAD 1
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, -size, 0)) );

        // QUAD 2
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, -size, 0)) );

        // QUAD 3
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, size, 0)) );

        // QUAD 4
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, -size, 0)) );

    }

    // GENERATE QUADS FOR LAST TWO POINTS OF TRAJECTORY
    {
        // ROTATION FOR FIRST POINT
        firstPoint = points.at(points.size()-2);
        secondPoint = points.at(points.size()-1);

        relativeVector = points.at(points.size()-1) - points.at(points.size()-3);

        angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
        quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
        angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
        quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

        quat_summ_first = (quat_yaw * quat_pitch).normalized();

        relativeVector = points.at(points.size()-1) - points.at(points.size()-2);

        angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
        quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
        angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
        quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

        quat_summ_second = quat_yaw * quat_pitch;

        // CREATING QUADS
        // QUAD 1
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, -size, 0)) );

        // QUAD 2
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, -size, 0)) );

        // QUAD 3
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, size, 0)) );

        // QUAD 4
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(-size, -size, 0)) );
        pointsCloud.append( firstPoint + quat_summ_first.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(size, -size, 0)) );
        pointsCloud.append( secondPoint + quat_summ_second.rotatedVector(QVector3D(-size, -size, 0)) );

    }


    // GENERATE TEXTURE COORDS
    foreach (QVector3D point, pointsCloud) {
        textureCoords.append(QVector2D(0.1, 0.25));
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
