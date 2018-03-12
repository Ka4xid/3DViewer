#include "object_builder.h"

Object_Builder::Object_Builder(QGLWidget* GLWidget)
{
    this->GLWidget = GLWidget;
    newObject = new Object_class;
}

Object_Builder* Object_Builder::SetName(QString name)
{
    newObject->name = name;
    return this;
}
Object_Builder* Object_Builder::SetTranslation(QVector3D translation)
{
    newObject->translation = translation;
    return this;
}
Object_Builder* Object_Builder::SetRotation(QVector3D rotation)
{
    newObject->rotation = rotation;
    return this;
}
Object_Builder* Object_Builder::SetScale(QVector3D scale)
{
    newObject->scale = scale;
    return this;
}
Object_Builder* Object_Builder::SetDrawType(uint newDrawType)
{
    newObject->polygonType = newDrawType;
    return this;
}
Object_Builder* Object_Builder::SetPointsData(QVector<float> p_Data, QVector<uint> p_Indices)
{
    newObject->SetPointsData(p_Data,
                             p_Indices);
    return this;
}
Object_Builder* Object_Builder::SetShaderPath(QString newShaderPath)
{
    newObject->shaderPath = newShaderPath;
    return this;
}
Object_Builder* Object_Builder::AddShaderValue(QString name, float value)
{
    newObject->shaderValues.insert(name, value);
    return this;
}
Object_Builder* Object_Builder::SetShaderValues(QMap<QString, float> newShaderValues)
{
    newObject->shaderValues = newShaderValues;
    return this;
}
Object_Builder* Object_Builder::SetTexturePath(QString newTexturePath)
{
    newObject->texturePath = newTexturePath;
    return this;
}
Object_Builder* Object_Builder::SetModelFile(QString newFilePath)
{
    fileToParsePath = newFilePath;
    return this;
}

void Object_Builder::Build()
{
    ply_parser* Parser = new ply_parser(fileToParsePath);

    QObject::connect(Parser, SIGNAL(DataReady(QVector<float>,QVector<uint>)),
            this, SLOT(SetPointsData(QVector<float>,QVector<uint>)));
    QObject::connect(Parser, SIGNAL(ReadComplete()),
            this, SLOT(Done()), Qt::QueuedConnection );

    Parser->ReadData();
}

void Object_Builder::Done()
{
    emit ObjectReady(newObject);
    this->deleteLater();
}

Object_class* Object_Builder::GetObject()
{
    return newObject;
}
