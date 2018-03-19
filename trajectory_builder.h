#ifndef TRAJECTORY_BUILDER_H
#define TRAJECTORY_BUILDER_H

#include <QObject>
#include <QVector3D>
#include "object_class.h"

class trajectory_builder : public QObject
{
    Q_OBJECT
public:
    trajectory_builder(QObject *parent = 0);

public slots:
    trajectory_builder* SetName(QString name);
    trajectory_builder* SetPoints(QVector<QVector3D> points);
    trajectory_builder* SetRadius(uint radius);
    void Build();

private slots:
    void SetPointsData(QVector<float> p_Data, QVector<uint> p_Indices);

private:
    void BuildTrajectory();
    void CreateQuadsForPoints(QVector3D point_prev, QVector3D point,
                              QVector3D point_next);
    uint radius;

    Object_class* newObject;

    QVector<QVector3D> trajectory_points;
    QVector<float> p_Data;
    QVector<uint> p_Indices;

    int even;

signals:
    void ObjectReady(Object_class* Object);
    void PointsDataReady(QVector<float> p_Data, QVector<uint> p_Indices);

};

#endif // TRAJECTORY_BUILDER_H
