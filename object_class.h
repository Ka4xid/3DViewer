#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H
#define USE_SHADERS

#include <QObject>
#include <QGLBuffer>
#include <QGLShader>
#include <QGLFunctions>


typedef void (APIENTRY *_glGenVertexArrays)(GLsizei, GLuint*);
typedef void (APIENTRY *_glBindVertexArray)(GLuint);


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
    void SetPointsVector(QVector<QVector3D> p_Coords,
                         QVector<QVector3D> p_Normals,
                         QVector<QVector2D> p_Texels,
                         QVector<uint> p_indices );
    void SetPointsVector(QVector<float> p_Data,
                         QVector<uint> p_Indices );

    void SetPointsArray(QVector<QVector3D> pointsCloud);
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

    // QT crutch, because this functions are not implemented in QOpenGlFunctions
    _glGenVertexArrays glGenVertexArrays = NULL;
    _glBindVertexArray glBindVertexArray = NULL;


    QVector3D translation,
              rotation,
              scale;

    uint polygonType;

    QGLBuffer* pointsCloud;
    QGLBuffer* normalsCloud;
    QGLBuffer* textureCloud;

    GLuint VAO, VBO, EBO;
    QGLBuffer* vertexData;
    QGLBuffer* vertexIndices;

    QVector<float> p_Data;
    QVector<uint> p_Indices;

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
