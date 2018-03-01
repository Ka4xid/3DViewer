#ifndef SURFACEBUILDER_H
#define SURFACEBUILDER_H

#include <QObject>
#include "parser.h"
#include <QVector3D>
#include "object_class.h"


class SurfaceBuilder : public QObject
{
    Q_OBJECT

public:
    SurfaceBuilder(QObject *parent = 0);

    int GenerateSurface(QString mtwFilePath, QString mapFilePath, uint textureScale = 100000 ); // Build surface mesh, based on height map
    void CreateMTW();

signals:

    //void ArraysReady(QVector<QVector3D> pointsCloud, QVector<QVector2D> textureCloud);
    //void TextureReady(QString textureFilePath);

    void SurfaceReady(Object_class* Object);

    void SetCameraPos(float x, float y, float z);

    void meshReady();
    void textureReady();


private slots:

    void ReceiveHeightsArray(QVector<float> heightsArray);

    void CreateObject();

    void CheckFlags();

private:

    QString mapFilePath,
            textureFilePath;

    QVector<QVector3D> pointsCloud;
    QVector<QVector3D> normalsCloud;
    QVector<QVector2D> textureCloud;

    bool textureReadyFlag;
    bool meshReadyFlag;

    uint textureScale;

    Parser* parser;


    int ReadInitialData(QString mapFilePath);
    void FormatHeightsToMesh(QVector<float> heightsArray);
    void generateTexture(double leftTopX, double leftTopY, double rightBottomX, double rightBottomY);

};


#endif // SURFACEBUILDER_H
