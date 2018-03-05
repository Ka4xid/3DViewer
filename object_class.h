#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H
//#define USE_SHADERS

#include <QObject>
#include <QGLBuffer>
#include <QGLShader>
#include <QGLFunctions>



class Object_class : public QObject, private QGLFunctions
{
    Q_OBJECT
public:
    Object_class(QObject *parent = 0);

    void SetName(QString newName);
    QString GetName();

    // TRANSLATION
    void SetTranslation(QVector3D newTranslation,
                        bool relatively = false);
    QVector3D GetTranslation();

    // ROTATION
    void SetRotation(QVector3D newRotation,
                     bool relatively = false);
    QVector3D GetRotation();

    // SCALE
    void SetScale(QVector3D newScale,
                  bool relatively = false);
    QVector3D GetScale();

    // POLYGON MODE
    void SetPolygonType(uint newPolygonType);
    uint GetPolygonType();

    //
    void SetPointsArray(QVector<QVector3D> newPointsVectors);
    void SetNormalsArray(QVector<QVector3D> newNormalsVectors);
    void SetTexturesArray(QVector<QVector2D> newTextureVectors);
#ifdef USE_SHADERS
    void SetShader(QString newShaderPath);
    void SetShaderValue(QString valueName, float value);
#endif

    void SetTexturePath(QString newTexturePath);

    void Initialize(QGLWidget* context);

    void Draw();

    void Delete();


private:

    QString name;
    QString texturePath;

    QGLWidget *context;

    QVector3D translation,
              rotation,
              scale;

    uint polygonType;

    QGLBuffer* pointsCloud;
    QGLBuffer* normalsCloud;
    QGLBuffer* textureCloud;

    QVector<QVector2D> texcoords;

    uint numberOfPoints;
#ifdef USE_SHADERS
    QString shaderPath;
    QGLShaderProgram *shader;
    QMap<QString, float> shaderValues;
#endif

    GLuint texture;


    GLuint v_attrib_position;
};

#endif // OBJECT_CLASS_H
