#include "surface_builder.h"
#include <QtConcurrentRun>
#include <QDir>
#include <QDateTime>

#include "mapapi.h"


Surface_Builder::Surface_Builder(QObject *parent) : QObject(parent)
{
    parser = new Parser();

    connect(parser, SIGNAL(HeightsArrayReady(QVector<float>)),
            this,   SLOT(ReceiveHeightsArray(QVector<float>)) );

    connect(this, SIGNAL(meshReady()),
            this, SLOT(CheckFlags()) );
    connect(this, SIGNAL(textureReady()),
            this, SLOT(CheckFlags()) );

    textureReadyFlag = 0;
    meshReadyFlag = 0;
}

int Surface_Builder::GenerateSurface(QString mtwFilePath, QString mapFilePath, uint textureScale)
{
    this->mapFilePath = mapFilePath;
    this->textureScale = textureScale;

    if ( ReadInitialData(mtwFilePath) ) {

        QtConcurrent::run(this, &Surface_Builder::generateTexture,
                                parser->Image.leftTopX,
                                parser->Image.leftTopY,
                                parser->Image.leftTopX + parser->Image.rowCount * parser->Image.elementMeters,
                                parser->Image.leftTopY + parser->Image.colCount * parser->Image.elementMeters );
        QtConcurrent::run(parser, &Parser::GetlHeightsArray);
        return  1;
    } else {
        return -1;
    }
}

int Surface_Builder::ReadInitialData(QString mapFilePath)
{
    if (parser->OpenFile(mapFilePath)) {
        if (parser->Parse(false)) {
            return 1;
        }
    }
    return -1;
}

void Surface_Builder::ReceiveHeightsArray(QVector<float> heightsArray)
{
    QtConcurrent::run(this, &Surface_Builder::FormatHeightsToMesh, heightsArray);
}

void Surface_Builder::FormatHeightsToMesh(QVector<float> heightsArray)
{

    // FILL POINTS CLOUD WITH 3D VECTORS
    float x=0,
          y=0;

    x = parser->Image.rowCount;  // because points start from left-top corner of map

    // FIL ARRAY WITH UNIQUE POINTS
    foreach (float point_height, heightsArray) {

        if (y > parser->Image.colCount-1 ) {
            y = 0;
            x -= 1;
        }

        // position
        p_Data.append( x * parser->Image.elementMeters );
        p_Data.append( y * parser->Image.elementMeters );
        p_Data.append( point_height );
        // normals
        p_Data.append( 0 );
        p_Data.append( 0 );
        p_Data.append( 1 );
        // texels
        p_Data.append( y / parser->Image.colCount );
        p_Data.append( x / parser->Image.rowCount );

        y += 1;
    }
    uint q=0;

    for (uint n=0; n<(p_Data.size()/8-parser->Image.colCount); n++)
    {
        if (q == parser->Image.colCount-1)      // OPTIMIZE ME
        {
            q=0;
            continue;
        }

        p_Indices.append(n);
        p_Indices.append(n + 1);
        p_Indices.append(n + parser->Image.colCount);

        p_Indices.append(n+1);
        p_Indices.append(n + parser->Image.colCount);
        p_Indices.append(n + parser->Image.colCount+1);

        q++;
    }

    qDebug() << "MESH READY";

    this->meshReadyFlag = true;
    emit meshReady();
}

void Surface_Builder::generateTexture(double leftTopX, double leftTopY, double rightBottomX, double rightBottomY)
{
    // PANORAMA's code, I dont even know what they are doing there
    HMAP hMap = 0;
    RECT RectDraw;

    int width, height;

    hMap = mapOpenData( this->mapFilePath.toUtf8().data() );

    if (hMap)
    {
        QPainter p;

        mapSetRealShowScale(hMap, textureScale);

        mapPlaneToPicture(hMap, &leftTopX, &leftTopY);
        mapPlaneToPicture(hMap, &rightBottomX, &rightBottomY);

        width = rightBottomX - leftTopX;
        height = leftTopY - rightBottomY;

        RectDraw.left = (int)leftTopX;
        RectDraw.top  = (int)leftTopY;
        RectDraw.right  = (int)leftTopX + (int)width;
        RectDraw.bottom = (int)leftTopY + (int)height;

        // Количество бит, выделяемое на одну точку изображения карты
        int  mapdepth = 32;
        XIMAGEDESC Ximagedesc;

        // Дополнить до mapdepth байт
        long allignwidth = width * (mapdepth / 8);
        long size   = allignwidth*height;
        char * bits = AllocateTheMemory(size);

        if (!bits) return;
        memset(bits, 0x0, size);

        Ximagedesc.Point     = bits;                        // Адрес начала области пикселов
        Ximagedesc.Width     = width;                       // Ширина строки в пикселах
        Ximagedesc.Height    = height;                      // Число строк
        Ximagedesc.Depth     = mapdepth;                    // Размер элемента в битах (8,15,16,24,32)
        Ximagedesc.CellSize  = mapdepth / 8;                // Размер элемента(пиксела) в байтах
        Ximagedesc.RowSize   = width * Ximagedesc.CellSize; // Ширина строки в байтах

        // Отобразим фрагмент карты в памяти
        mapPaintToXImage(hMap, &Ximagedesc, 0, 0, &RectDraw);

        QImage image((uchar *)bits, width, height, QImage::Format_RGB32);

        p.drawImage(leftTopX, leftTopY, image, width, height);


        QDir dir;
        QString filePath = dir.absolutePath().append("/surface_texture.png");

        if (image.save(filePath) ) {
            qDebug() << "TEXTURE SAVED";
            this->textureFilePath = filePath;
        } else {
            this->textureFilePath = ":/3D_viewer/chkr.png";
            qDebug() << "TEXTURE SAVE FAILED";
        }

        FreeTheMemory(bits);
    }
    this->textureReadyFlag = true;
    emit textureReady();
}

void Surface_Builder::CheckFlags()
{
    if (textureReadyFlag && meshReadyFlag) {
        CreateObject();
    }
}

void Surface_Builder::CreateObject()
{
    Object_class* Map = new Object_class;

    Map->name = "Map";
    Map->translation = QVector3D(parser->Image.leftBottomX, parser->Image.leftBottomY, 0);
    Map->SetPointsData(p_Data, p_Indices);
    Map->scale = QVector3D(1,1,1);
    Map->shaderPath = ":3D_viewer/Shaders/default";
    Map->texturePath = textureFilePath;

    qDebug() << "MAP READY";

    emit SetCameraPos(QVector3D(parser->Image.leftBottomX + (parser->Image.rowCount * parser->Image.elementMeters / 2),
                                parser->Image.leftBottomY + (parser->Image.colCount * parser->Image.elementMeters / 2),
                                parser->Image.minHeight + (parser->Image.maxHeight - parser->Image.minHeight)/2) );

    emit SurfaceReady(Map);

    parser->deleteLater();
    this->deleteLater();
}

void Surface_Builder::CreateMTW()
{
    // FOR DEBUG PURPOSES

    HMAP hMap = 0;
    BUILDMTW mtw;


    hMap = mapOpenData( "/home/fuego/Podolsk/Podolsk.map" );

    if (hMap) {

        memset(&mtw, 0x0, sizeof(BUILDMTW) );

        double x1, y1, x2, y2;

        x1 = mapGetMapX1(hMap);
        y1 = mapGetMapY1(hMap);
        x2 = mapGetMapX2(hMap);
        y2 = mapGetMapY2(hMap);

        qDebug() << x1 << y1;
        qDebug() << x2 - x1 << y2 -y1;

        mtw.StructSize = sizeof(BUILDMTW);

        mtw.BeginX = x1;
        mtw.BeginY = y1;
        mtw.Width = 33297; //(y2 - y1);
        mtw.Height = 39211; //(x2 - x1);

        mtw.ElemSizeMeters = 30;
        mtw.ElemSizeBytes = 4;
        mtw.Unit = 3;
        mtw.ReliefType = 0;

        memset(mtw.Reserve, 0x0, 20);

        mtw.Method = 0;
        mtw.Extremum = 0;


        qDebug() << mapBuildMtw(hMap, "/home/fuego/Podolsk/Podolsk.mtw", 0, &mtw, 0);
    }

    mapCloseData(hMap);

}
