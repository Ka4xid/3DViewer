#include "object_class.h"
#include <qmath.h>
#include <QDateTime>

Object_class::Object_class(QObject *parent) : QObject(parent)
{
    initializeGLFunctions();
    this->name = "na";

    this->translation = QVector3D(0,0,0);
    this->rotation = QVector3D(0,0,0);
    this->scale = QVector3D(1,1,1);

    this->polygonType = GL_TRIANGLES;
    this->numberOfPoints = 0;

    this->vertexData = new QGLBuffer(QGLBuffer::VertexBuffer);
    this->vertexData->create();
    this->vertexData->bind();
    this->vertexData->setUsagePattern(QGLBuffer::StaticDraw);
    this->vertexData->release();

    this->vertexIndices = new QGLBuffer(QGLBuffer::IndexBuffer);
    this->vertexIndices->create();
    this->vertexIndices->bind();
    this->vertexIndices->setUsagePattern(QGLBuffer::StaticDraw);
    this->vertexIndices->release();
    //

    this->texturePath = ":/3D_viewer/checker.png";

    this->shaderPath = ":/3D_viewer/Shaders/default";

    this->shader = new QGLShaderProgram(this);
    this->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                 QString(":/3D_viewer/Shaders/default.vert"));
    this->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                 QString(":/3D_viewer/Shaders/default.frag"));
    this->shader->link();

    this->shaderValues = QMap<QString, float>();

    this->objectTime;
}

void Object_class::SetPointsData(QVector<float> p_Data,
                                 QVector<uint> p_Indices )
{
    this->p_Data = p_Data;
    this->p_Indices = p_Indices;

    this->numberOfPoints = p_Indices.size();
}

void Object_class::CompileShader()
{
    int result=0;

    this->shader->removeAllShaders();

    this->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                          QString("%1.%2").arg(this->shaderPath, "vert")) ? (result++) : 0;
    this->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                          QString("%1.%2").arg(this->shaderPath, "frag")) ? (result++) : 0;
    this->shader->link() ? (result++) : 0;

    // IF COMPILATION FAILED
    if (result!=3) {
        // USE DEFAULT SHADER
        qDebug() << "Shader compilation failed, using default shader";
        this->shader->removeAllShaders();        
        this->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                     QString(":/3D_viewer/Shaders/default.vert"));
        this->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                     QString(":/3D_viewer/Shaders/default.frag"));
        this->shader->link();
    }
}

void Object_class::Delete()
{
    // FIXME

    delete this->shader;

    delete this;
}


void Object_class::Initialize(QGLWidget *context)
{
    CompileShader();

    texture = context->bindTexture(texturePath, GL_TEXTURE_2D);

    glGenVertexArrays = (_glGenVertexArrays)context->context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray)context->context()->getProcAddress("glBindVertexArray");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

        this->vertexData->bind();
        this->vertexData->allocate( p_Data.constData(), p_Data.size() * sizeof(float) );
        this->p_Data.clear();

        this->vertexIndices->bind();
        this->vertexIndices->allocate( p_Indices.constData(), p_Indices.size() * sizeof(uint) );
        this->p_Indices.clear();

        // Position
        int pos_location = this->shader->attributeLocation("Vert_Pos");
        if (pos_location != -1)
        {
            glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
            glEnableVertexAttribArray(pos_location);
        }

        // Normals
        int normals_location = this->shader->attributeLocation("Vert_Normal");
        if (normals_location != -1)
        {
            glVertexAttribPointer(normals_location, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(normals_location);
        }

        // Texels
        int texels_location = this->shader->attributeLocation("Vert_Texel");
        if (texels_location != -1) {
            glVertexAttribPointer(texels_location, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)) );
            glEnableVertexAttribArray(texels_location);
        }

    glBindVertexArray(0);
}

void Object_class::Draw()
{
    // Object transformations
    glTranslatef(this->translation.x(),
                 this->translation.y(),
                 this->translation.z() );

    glRotatef(this->rotation.z(), 0, 0, 1);
    glRotatef(this->rotation.y(), 0, 1, 0);
    glRotatef(this->rotation.x(), 1, 0, 0);

    glScalef(this->scale.x(),
             this->scale.y(),
             this->scale.z() );

    // Object rendering
    this->shader->bind();

    this->shader->setAttributeValue("time", objectTime);

    foreach (QString key, this->shaderValues.keys()) {
        this->shader->setAttributeValue(key.toUtf8().constData(), this->shaderValues.value(key, 0) );
    }

    glBindTexture(GL_TEXTURE_2D, texture);

    glBindVertexArray(VAO);
    glDrawElements(this->polygonType, numberOfPoints, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    this->shader->release();

    objectTime+=0.1;
}
