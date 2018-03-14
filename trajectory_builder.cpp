#include "trajectory_builder.h"
#include <qmath.h>
#include <QVector>
#include <QVector2D>
#include <QQuaternion>

#include <QDebug>

trajectory_builder::trajectory_builder(QObject *parent) : QObject(parent)
{
    newObject = new Object_class();
    newObject->polygonType = GL_QUADS;
    newObject->shaderPath = ":3D_viewer/Shaders/trajectory";
}

trajectory_builder* trajectory_builder::SetName(QString name)
{
    newObject->name = name;
    return this;
}

trajectory_builder* trajectory_builder::SetPoints(QVector<QVector3D> points)
{
    this->trajectory_points = points;
    return this;
}

trajectory_builder* trajectory_builder::SetRadius(uint radius)
{
    this->radius = radius;
    return this;
}

void trajectory_builder::SetPointsData(QVector<float> p_Data, QVector<uint> p_Indices)
{
    newObject->SetPointsData(p_Data, p_Indices);

    emit ObjectReady(newObject);

    return;
}

void trajectory_builder::Build()
{

    connect(this, SIGNAL(PointsDataReady(QVector<float>,QVector<uint>)),
            this, SLOT(SetPointsData(QVector<float>,QVector<uint>)), Qt::QueuedConnection);

    BuildTrajectory();

    return;
}

void trajectory_builder::BuildTrajectory()
{
    // GENERATE QUADS FOR FIRST TWO POINTS OF TRAJECTORY
    CreateQuadsForPoints(trajectory_points.at(0), trajectory_points.at(0),
                         trajectory_points.at(1), trajectory_points.at(2));


    // GENERATE QUADS FOR MIDDLE POINTS OF TRAJECTORY
    for (int i=1; i<(trajectory_points.size()-2); i++) {
        CreateQuadsForPoints(trajectory_points.at(i-1), trajectory_points.at(i),
                             trajectory_points.at(i+1), trajectory_points.at(i+2));
    }

    // GENERATE QUADS FOR LAST TWO POINTS OF TRAJECTORY
    CreateQuadsForPoints(trajectory_points.at(trajectory_points.size()-3), trajectory_points.at(trajectory_points.size()-2),
                         trajectory_points.at(trajectory_points.size()-1), trajectory_points.at(trajectory_points.size()-1));

    for (int n=0; n<(p_Data.size()/8); n++)
    {
        p_Indices.append(n);
    }



    emit PointsDataReady(p_Data, p_Indices);
}

void trajectory_builder::CreateQuadsForPoints(QVector3D point0, QVector3D point1,
                                              QVector3D point2, QVector3D point3)
{
    QVector3D v;
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
    int sides_number = 6;

    // FIND RADIANS IN ONE QUAD
    float radians = M_PI / (sides_number/2);

    // CREATE QUADS FOR SIDES
    for (int n=0; n<sides_number; n++) {

        // GENERATE POSITION 1
            v = point1 + quat1.rotatedVector(QVector3D(cos(radians * n) * radius, sin(radians * n) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE NORMALS 1
            v = quat1.rotatedVector(QVector3D(cos(radians * n) * radius, sin(radians * n) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE TEXTURE COORDS 1
            p_Data.append(1);
            p_Data.append(0);


        // GENERATE POSITION 2
            v = point1 + quat1.rotatedVector(QVector3D(cos(radians * (n+1)) * radius, sin(radians * (n+1)) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        //GENERATE NORMALS 2
            v = quat1.rotatedVector(QVector3D(cos(radians * (n+1)) * radius, sin(radians * (n+1)) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE TEXTURE COORDS 2
            p_Data.append(0);
            p_Data.append(0);


        // GENERATE POSITION 3
            v = point2 + quat2.rotatedVector(QVector3D(cos(radians * (n+1)) * radius, sin(radians * (n+1)) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        //GENERATE NORMALS 3
            v = quat2.rotatedVector(QVector3D(cos(radians * (n+1)) * radius, sin(radians * (n+1)) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE TEXTURE COORDS 3
            p_Data.append(0);
            p_Data.append(1);


        // GENERATE POSITION 4
            v = point2 + quat2.rotatedVector(QVector3D(cos(radians * n) * radius, sin(radians * n) * radius, 0)) ;
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        //GENERATE NORMALS 4
            v = quat2.rotatedVector(QVector3D(cos(radians * n) * radius, sin(radians * n) * radius, 0)) ;
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE TEXTURE COORDS 4
            p_Data.append(1);
            p_Data.append(1);

    }
}
