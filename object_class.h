#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H
#define USE_SHADERS

#include <QObject>
#include <QGLBuffer>
#include <QGLShader>
#include <QGLFunctions>


typedef void (APIENTRY *_glGenVertexArrays)(GLsizei, GLuint*);
typedef void (APIENTRY *_glBindVertexArray)(GLuint);
typedef void (APIENTRY *_glDeleteVertexArrays)(GLsizei, GLuint*);

class Object_class : public QObject, private QGLFunctions
{
    Q_OBJECT
public:
    Object_class(QObject *parent = 0);

    void SetPointsData(QVector<float> p_Data,
                       QVector<uint> p_Indices );

    void Initialize(QGLWidget* context);

    void Draw();

    void Delete();


public:

    QString name;

    QVector3D translation,
              rotation,
              scale;

    uint polygonType;
    QString shaderPath;
    QString texturePath;

    QMap<QString, float> shaderValues;


private:
    void CompileShader();

    // Kludge, because this functions are not implemented in QOpenGlFunctions version 4.8
    _glGenVertexArrays glGenVertexArrays = NULL;
    _glBindVertexArray glBindVertexArray = NULL;
    _glDeleteVertexArrays glDeleteVertexArrays = NULL;


    GLuint VAO;
    QGLBuffer* vertexData;
    QGLBuffer* vertexIndices;

    QVector<float> p_Data;
    QVector<uint> p_Indices;

    uint numberOfPoints;

    QGLShaderProgram *shader;

    GLuint texture;

    float objectTime;
};

#endif // OBJECT_CLASS_H
