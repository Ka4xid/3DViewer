#include "object_class.h"


Object_class::Object_class(QObject *parent) : QObject(parent)
{
    initializeGLFunctions();
    this->name = "na";

    this->translation = QVector3D(0,0,0);
    this->rotation = QVector3D(0,0,0);
    this->scale = QVector3D(1,1,1);

    this->polygonType = GL_TRIANGLES;
    this->numberOfPoints = 0;

    // ESTABLISH BUFFERS;
    this->pointsCloud = new QGLBuffer(QGLBuffer::VertexBuffer);
    this->pointsCloud->create();
    this->pointsCloud->bind();
    this->pointsCloud->setUsagePattern(QGLBuffer::DynamicDraw);
    this->pointsCloud->release();

    this->normalsCloud = new QGLBuffer(QGLBuffer::VertexBuffer);
    this->normalsCloud->create();
    this->normalsCloud->bind();
    this->normalsCloud->setUsagePattern(QGLBuffer::DynamicDraw);
    this->normalsCloud->release();

    this->textureCloud = new QGLBuffer(QGLBuffer::VertexBuffer);
    this->textureCloud->create();
    this->textureCloud->bind();
    this->textureCloud->setUsagePattern(QGLBuffer::DynamicDraw);
    this->textureCloud->release();

    // Create buffers for vertecies data and indices;

    this->glGenBuffers(1, &VAO);

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

    this->texturePath = ":/3D_viewer/chkr.png";

#ifdef USE_SHADERS
    this->shaderPath = "/3D_viewer/Shaders/default";

    this->shader = new QGLShaderProgram(this);

    this->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                 QString(":%1.%2").arg("/3D_viewer/Shaders/default", "vert"));
    this->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                 QString(":%1.%2").arg("/3D_viewer/Shaders/default", "frag"));
    this->shader->bindAttributeLocation("texture_coord", this->textureCloud->bufferId());
    this->shader->link();

    this->shaderValues = QMap<QString, float>();
#endif

}


void Object_class::SetName(QString newName)
{
    this->name = newName;
}
QString Object_class::GetName()
{
    return this->name;
}

void Object_class::SetTranslation(QVector3D newTranslation, bool relatively)
{
    if (relatively) {
        this->translation += newTranslation;
    } else {
        this->translation = newTranslation;
    }

}
QVector3D Object_class::GetTranslation()
{
    return translation;
}

void Object_class::SetRotation(QVector3D newRotation, bool relatively)
{
    if (relatively) {
        this->rotation += newRotation;
    } else {
        this->rotation = newRotation;
    }
}
QVector3D Object_class::GetRotation()
{
    return this->rotation;
}

void Object_class::SetScale(QVector3D newScale, bool relatively)
{
    if (relatively) {
        this->scale += newScale;
    } else {
        this->scale = newScale;
    }
}
QVector3D Object_class::GetScale()
{
    return this->scale;
}

void Object_class::SetPolygonType(uint newPolygonType)
{
    this->polygonType = newPolygonType;
}
uint Object_class::GetPolygonType()
{
    return this->polygonType;
}

void Object_class::SetPointsVector(QVector<QVector3D> p_Coords,
                                   QVector<QVector3D> p_Normals,
                                   QVector<QVector2D> p_Texels,
                                   QVector<uint> p_indices )
{
    // transform data from OBJ file?
    // (or do it in parser)
}

void Object_class::SetPointsVector(QVector<float> p_Data,
                                   QVector<uint> p_Indices )
{
    /*this->vertexData->bind();
    this->vertexData->allocate( p_Data.constData(), p_Data.size() * sizeof(float) );
    this->vertexData->release();

    this->vertexIndices->bind();
    this->vertexIndices->allocate( p_Indices.constData(), p_Indices.size() * sizeof(uint) );
    this->vertexIndices->release();*/

    this->p_Data = p_Data;
    this->p_Indices = p_Indices;

    this->numberOfPoints = p_Indices.size();
}

void Object_class::SetPointsArray(QVector<QVector3D> newPointsVectors)
{
    QVector<float> tempPointsArray;
    numberOfPoints = newPointsVectors.size();

    foreach (QVector3D point, newPointsVectors) {
        tempPointsArray.append(point.x());
        tempPointsArray.append(point.y());
        tempPointsArray.append(point.z());
    }

    this->pointsCloud->bind();
    this->pointsCloud->allocate( tempPointsArray.size() * sizeof(float) );
    this->pointsCloud->write(0, tempPointsArray.data(), tempPointsArray.size() * sizeof(float));
    this->pointsCloud->release();
}

void Object_class::SetNormalsArray(QVector<QVector3D> newNormalsVectors)
{
    QVector<float> tempNormalsArray;

    foreach (QVector3D normal, newNormalsVectors) {
        tempNormalsArray.append(normal.x());
        tempNormalsArray.append(normal.y());
        tempNormalsArray.append(normal.z());
    }

    this->normalsCloud->bind();
    this->normalsCloud->allocate( tempNormalsArray.size() * sizeof(float));
    this->normalsCloud->write(0, tempNormalsArray.data(), tempNormalsArray.size() * sizeof(float));
    this->normalsCloud->release();
}

void Object_class::SetTexturesArray(QVector<QVector2D> newTextureVectors)
{
    QVector<float> tempTexturesArray;

    texcoords = newTextureVectors;

    foreach (QVector2D textureCoord, newTextureVectors) {
        tempTexturesArray.append(textureCoord.x());
        tempTexturesArray.append(textureCoord.y());
    }

    this->textureCloud->bind();
    this->textureCloud->allocate( tempTexturesArray.size() * sizeof(float) );
    this->textureCloud->write(0, tempTexturesArray.data(), tempTexturesArray.size() * sizeof(float) );
    this->textureCloud->release();
}

#ifdef USE_SHADERS
void Object_class::SetShader(QString newShaderPath)
{
    int result=0;

    this->shader->removeAllShaders();

    this->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                          QString(":%1.%2").arg(newShaderPath, "vert")) ? (result++) : 0;
    this->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                          QString(":%1.%2").arg(newShaderPath, "frag")) ? (result++) : 0;
    this->shader->link() ? (result++) : 0;

    // IF COMPILATION FAILED
    if (result!=3) {
        // USE DEFAULT SHADER
        this->shader->removeAllShaders();
        qDebug() << "Using default shader";
        this->shader->addShaderFromSourceFile(QGLShader::Vertex,
                                     QString(":%1.%2").arg("/3D_viewer/Shaders/default", "vert"));
        this->shader->addShaderFromSourceFile(QGLShader::Fragment,
                                     QString(":%1.%2").arg("/3D_viewer/Shaders/default", "frag"));
        this->shader->link();
    }
}

void Object_class::SetShaderValue(QString valueName, float value)
{
    shaderValues.insert(valueName, value);
}
#endif

void Object_class::SetTexturePath(QString newTexturePath)
{
    this->texturePath = newTexturePath;
}

void Object_class::Initialize(QGLWidget *context)
{
    texture = context->bindTexture(QImage(texturePath), GL_TEXTURE_2D);

    qDebug() << glGenVertexArrays;
    glGenVertexArrays = (_glGenVertexArrays)context->context()->getProcAddress("glGenVertexArrays");
    glBindVertexArray = (_glBindVertexArray)context->context()->getProcAddress("glBindVertexArray");
    qDebug() << glGenVertexArrays;


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

        this->vertexData->bind();
        this->vertexData->allocate( p_Data.constData(), p_Data.size() * sizeof(float) );

        this->vertexIndices->bind();
        this->vertexIndices->allocate( p_Indices.constData(), p_Indices.size() * sizeof(uint) );

        // Position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // Normals
        /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)) );
        glEnableVertexAttribArray(1);*/
        // Texels
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)) );
        glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Object_class::Draw()
{
    glTranslatef(this->translation.x(),
                 this->translation.y(),
                 this->translation.z() );

    glRotatef(this->rotation.z(), 0, 0, 1);
    glRotatef(this->rotation.y(), 0, 1, 0);
    glRotatef(this->rotation.x(), 1, 0, 0);

    glScalef(this->scale.x(),
             this->scale.y(),
             this->scale.z() );



    glBindTexture(GL_TEXTURE_2D, texture);
#ifdef USE_SHADERS
    this->shader->bind();
#endif

    glBindVertexArray(VAO);
    glDrawElements(this->polygonType, numberOfPoints, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //this->vertexData->release();
    //this->vertexIndices->release();


    //this->normalsCloud->release();
    //this->pointsCloud->release();


#ifdef USE_SHADERS
    this->shader->release();
#endif
}

void Object_class::Delete()
{
    this->pointsCloud->destroy();
    delete this->pointsCloud;
    this->normalsCloud->destroy();
    delete this->normalsCloud;
    this->textureCloud->destroy();
    delete this->textureCloud;

#ifdef USE_SHADERS
    delete this->shader;
#endif

    delete this;
}
