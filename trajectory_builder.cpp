#include "trajectory_builder.h"
#include <qmath.h>
#include <QVector>
#include <QVector2D>
#include <QQuaternion>
#include <QDebug>

#define SIDES_NUMBER 8

trajectory_builder::trajectory_builder(QObject *parent) : QObject(parent)
{
    newObject = new Object_class();
    newObject->polygonType = GL_QUADS;
    newObject->shaderPath = ":3D_viewer/Shaders/trajectory";

    even = 0;
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
    if (trajectory_points.size() > 2) {
        even=1;
        CreateQuadsForPoints(trajectory_points.at(0),
                             trajectory_points.at(0),
                             trajectory_points.at(1) );

        for (int n=1; n < trajectory_points.size()-1; n++) {
            even++;
            CreateQuadsForPoints(trajectory_points.at(n-1),
                                 trajectory_points.at(n),
                                 trajectory_points.at(n+1) );
        }
        even++;
        CreateQuadsForPoints(trajectory_points.at(trajectory_points.size()-2),
                             trajectory_points.at(trajectory_points.size()-1),
                             trajectory_points.at(trajectory_points.size()-1) );
    }
    else if (trajectory_points.size() == 2) {
        CreateQuadsForPoints(trajectory_points.at(0),
                             trajectory_points.at(0),
                             trajectory_points.at(1) );
        even++;
        CreateQuadsForPoints(trajectory_points.at(0),
                             trajectory_points.at(1),
                             trajectory_points.at(1) );
    }
    else {
        qDebug() << "UNABLE TO BUILD TRAJECTORY FOR 1 POINT";
        return;
    }

    for (int i=0; i <= p_Data.size()/8-SIDES_NUMBER*2; i+=SIDES_NUMBER) {
        for(int n=0; n<SIDES_NUMBER-1; n++) {
            p_Indices.append(i+n);
            p_Indices.append(i+n+SIDES_NUMBER);
            p_Indices.append(i+n+SIDES_NUMBER+1);
            p_Indices.append(i+n+1);
        }
        p_Indices.append(i+(SIDES_NUMBER-1));
        p_Indices.append(i+(SIDES_NUMBER*2-1));
        p_Indices.append(i+SIDES_NUMBER);
        p_Indices.append(i);
    }

    emit PointsDataReady(p_Data, p_Indices);
}

void trajectory_builder::CreateQuadsForPoints(QVector3D point_previous, QVector3D point,
                                              QVector3D point_next)
{
    QVector3D v;
    QVector3D relativeVector;
    float angle;
    QQuaternion quat_yaw,
                quat_pitch,
                quat_summ;

    relativeVector = point_next - point_previous;

    // 57.2958 - MAGIC NUMBER TO CONVERT FROM RADIANS TO DEGREES
    angle = atan2(relativeVector.y(), relativeVector.x()) * 57.2958;
    quat_yaw = QQuaternion::fromAxisAndAngle(QVector3D(0,0,1), angle);
    angle = atan2(relativeVector.toVector2D().length(), relativeVector.z()) * 57.2958;
    quat_pitch = QQuaternion::fromAxisAndAngle(QVector3D(0,1,0), angle);

    quat_summ = (quat_yaw * quat_pitch).normalized();


    // TO MAKE MESH SMOOTHER, JUST ADJUST SIDES_NUMBER VALUE

    // FIND RADIANS IN ONE SIDE
    float radians = M_PI / (SIDES_NUMBER/2);

    // CREATE QUADS FOR SIDES
    for (int n=0; n<SIDES_NUMBER; n++) {

        // GENERATE POSITION 1
            v = point + quat_summ.rotatedVector(QVector3D(cos(radians * n) * radius, sin(radians * n) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE NORMALS 1
            v = quat_summ.rotatedVector(QVector3D(cos(radians * n) * radius, sin(radians * n) * radius, 0));
            p_Data.append(v.x());
            p_Data.append(v.y());
            p_Data.append(v.z());
        // GENERATE TEXTURE COORDS 1
            p_Data.append(0);
            p_Data.append(0);

    }

}

