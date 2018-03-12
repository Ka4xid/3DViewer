#ifndef TRAJECTORY_BUILDER_H
#define TRAJECTORY_BUILDER_H

#include <QObject>

class trajectory_builder : public QObject
{
    Q_OBJECT
public:
    explicit trajectory_builder(QObject *parent = 0);

signals:

public slots:
};

#endif // TRAJECTORY_BUILDER_H
