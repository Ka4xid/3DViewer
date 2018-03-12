#ifndef OBJECT_BUILDER_H
#define OBJECT_BUILDER_H

#include <QObject>
#include "object_class.h"
#include "ply_parser.h"

class Object_Builder : public QObject
{
    Q_OBJECT
public:
    Object_Builder(QGLWidget* GLWidget);

    Object_class* GetObject();

    void ReadFile(QString filePath);

public slots:
    Object_Builder* SetName(QString name);
    Object_Builder* SetTranslation(QVector3D translation);
    Object_Builder* SetRotation(QVector3D rotation);
    Object_Builder* SetScale(QVector3D scale);
    Object_Builder* SetDrawType(uint newDrawType);
    Object_Builder* SetPointsData(QVector<float> p_Data,
                                 QVector<uint> p_Indices);
    Object_Builder* SetShaderPath(QString newShaderPath);
    Object_Builder* AddShaderValue(QString name, float value);
    Object_Builder* SetShaderValues(QMap<QString, float> newShaderValues);
    Object_Builder* SetTexturePath(QString newTexturePath);
    Object_Builder* SetModelFile(QString newFilePath);
    void Build();

    void Done();

private:
    QGLWidget* GLWidget;
    QString fileToParsePath;
    Object_class* newObject;

signals:
    void ObjectReady(Object_class* Object);

};

#endif // OBJECT_BUILDER_H
