#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QFile>
#include <QtGlobal>
#include <QDebug>
#include <stdint.h>


struct image_type {
    uint32_t fileLength;
    uint32_t elementSize;           // in bits
    uint32_t rowCount;              // image height in elements
    uint32_t colCount;              // image width in elements

    uint32_t blockRowCount;         // number of blocks in image
    uint32_t blockColCount;         // number of blocks in image
    uint32_t blockHeight;           // in elements
    uint32_t blockWidth;            // in elements
    uint32_t blockCutHeight;        // in elements
    uint32_t blockCutWidth;         // in elements
    uint32_t blockBeginDescOffset;
    uint32_t blockDescSize;

    float minHeight,              // in units
          maxHeight;              // in units

    float elementMeters;           // element size in meters
    float leftBottomX;             // X coord of left-bottom point in meters
    float leftBottomY;             // Y coord of left-bottom point in meters

    float leftTopX;
    float leftTopY;

    float measurementUnits;
};

struct block_type {
    QByteArray data;
    uint32_t row,
             col;
    uint32_t height,
             width;
};


class Parser : public QObject
{
    Q_OBJECT

public:

    Parser();

    int OpenFile(QString path);
    void CloseFile();
    int Parse(bool output);

    float GetHeightByElement(int32_t imageRow, int32_t imageCol);
    float GetHeightByPoint(double X, double Y );

    image_type Image;
    block_type Block;

    void GetlHeightsArray();

signals:

    void TextureCoordsReady(double x, double y, double width, double height);
    void HeightsArrayReady(QVector<float> heightsArray);

private:

    int32_t LoadBlock(uint row, uint col);
    float ReadElementInBlock(uint row, uint col);

    QFile mtwFile;

};

#endif // PARSER_H
