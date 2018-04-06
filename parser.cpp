#include "parser.h"

#define BAD_BLOCK   -10001
#define BAD_ELEMENT -10002
#define NO_HEIGHT -10003
#define NA -32767
//#define DEBUG_OUTPUT


Parser::Parser() : QObject()
{
}

int Parser::OpenFile(QString path) {

    mtwFile.setFileName(path);
    if ( mtwFile.open(QIODevice::ReadOnly) ) {
        return 1;
    }
    return -1;
}

void Parser::CloseFile()
{
    mtwFile.close();
}

int Parser::Parse(bool output)
{
    if (mtwFile.size() == 0) {
        return -1;
    }
    QDataStream read(&mtwFile);
    read.setByteOrder(QDataStream::LittleEndian);

    read.skipRawData(8);

    read >> Image.fileLength;

    read.skipRawData(40);

    read >> Image.elementSize;
    read >> Image.rowCount;
    read >> Image.colCount;
    read >> Image.blockColCount;
    read >> Image.blockRowCount;
    read >> Image.blockHeight;
    read >> Image.blockWidth;
    read >> Image.blockCutHeight;
    read >> Image.blockCutWidth;

    read.skipRawData(16);

    read >> Image.blockBeginDescOffset;
    read >> Image.blockDescSize;

    read.skipRawData(40);

    read >> Image.elementMeters;
    read >> Image.leftBottomX;
    read >> Image.leftBottomY;

    Image.leftTopX = Image.leftBottomX + Image.rowCount * Image.elementMeters;
    Image.leftTopY = Image.leftBottomY;

    mtwFile.seek(304);
    int32_t value;
    read >> value;
    switch (value) {
        case 0:
            Image.measurementUnits = 1;

        case 1:
            Image.measurementUnits = 0.1;
            break;
        case 2:
            Image.measurementUnits = 0.01;
            break;
        case 3:
            Image.measurementUnits = 0.001;
            break;
        default:
            break;
    }

    mtwFile.seek(280);
    read >> Image.minHeight;
    read >> Image.maxHeight;
    Image.minHeight = Image.minHeight * Image.measurementUnits;
    Image.maxHeight = Image.maxHeight * Image.measurementUnits;

    if (output) {

        qDebug() << "File Size: " <<  Image.fileLength;
        qDebug() << "Element Size: " <<  Image.elementSize;
        qDebug() << "Image Row Count: " << Image.rowCount;
        qDebug() << "Image Column Count: " << Image.colCount;
        qDebug() << "Block Row Count: " << Image.blockRowCount;
        qDebug() << "Block Column Count: " << Image.blockColCount;
        qDebug() << "Block Height: " << Image.blockHeight;
        qDebug() << "Block Width: " << Image.blockWidth;
        qDebug() << "Cut Block Height: " << Image.blockCutHeight;
        qDebug() << "Cut Block Width: " << Image.blockCutWidth;

        qDebug() << "Blocks Desc.Offset: " << Image.blockBeginDescOffset;
        qDebug() << "Blocks Desc.Size: " << Image.blockDescSize;

        qDebug() << "Meters in Element : " << Image.elementMeters;
        qDebug() << "0 point X : " << Image.leftBottomX;
        qDebug() << "0 point Y : " << Image.leftBottomY;

        qDebug() << "Min Height:" << Image.minHeight << "Max Height" << Image.maxHeight;

        qDebug() << Image.measurementUnits;

        qDebug() << "";
    }


    return 1;
}


int32_t Parser::LoadBlock(uint row, uint col)
{
    if (mtwFile.size() == 0) {
        return -1;
    }

    uint32_t blockIndex,
             blockDescOffset,
             blockOffset,
             blockSize;

    mtwFile.reset();                                // rewind file
    QDataStream read(&mtwFile);                     // setup data stream
    read.setByteOrder(QDataStream::LittleEndian);   // set byte order in stream


    blockIndex = row * Image.blockColCount + col;   // calculate required block index
    blockDescOffset = blockIndex * 8;               // calculate block offset


    read.skipRawData( (Image.blockBeginDescOffset + blockDescOffset) );     //
    read >> blockOffset;                                                    // read block offset
    read >> blockSize;                                                      // read block size
#ifdef DEBUG_OUTPUT
    qDebug() << blockOffset;
    qDebug() << blockSize;
#endif

    Block.data.reserve(blockSize);                                          // reserve space in block data structure
    mtwFile.seek(blockOffset);                                              //
    Block.data = mtwFile.read(blockSize);                                   // read data to block data structure
    Block.row = row;                                                        //
    Block.col = col;                                                        //

                                                                            // set proper width of height of block
    if ( (row == Image.blockRowCount-1)) {
        Block.height = Image.blockCutHeight;
    } else {
        Block.height = Image.blockHeight;
    }
    if ( (col == Image.blockColCount-1)) {
        Block.width = Image.blockCutWidth;
    } else {
        Block.width = Image.blockWidth;
    }

    return 1;
}


float Parser::GetHeightByPoint(double X, double Y)
{
    if (mtwFile.size() == 0) {
        return -1;
    }

    mtwFile.reset();
    QDataStream read(&mtwFile);
    read.setByteOrder(QDataStream::LittleEndian);

    uint32_t imageRow,
             imageCol,
             blockRow,
             blockCol,
             blockIndex,
             blockDescOffset,
             blockOffset,
             blockLength,
             elementRow,
             elementCol,
             elementOffset;

    //Step 1 - find element row and column in image coords
    imageRow = (Image.leftTopX - X) / Image.elementMeters;
    imageCol = (Y - Image.leftTopY) / Image.elementMeters;
#ifdef DEBUG_OUTPUT
    qDebug() << "Element in image Row:" << imageRow << "Col:" << imageCol ;
#endif
    if ( (imageRow > Image.rowCount) ||
         (imageCol > Image.colCount) ) {
        qDebug() << "BAD ADDRESS";
        return -1;
    }


    //Step 2 - find row and column of requested block
    blockRow = imageRow / Image.blockHeight;
    blockCol = imageCol / Image.blockWidth;
#ifdef DEBUG_OUTPUT
    qDebug() << "Block Row:" << blockRow << "Col:" << blockCol ;
#endif

    //Step 3 - find requested block index
    blockIndex = blockRow * Image.blockColCount + blockCol;
#ifdef DEBUG_OUTPUT
    qDebug() << "Block index:" << blockIndex ;
#endif


    //Step 4 - find offset to read specific block;
    blockDescOffset = Image.blockBeginDescOffset + blockIndex * 8;
#ifdef DEBUG_OUTPUT
    qDebug() << "Block Description offset:" << blockDescOffset;
#endif


    //Step 5 - find element offset and length
    read.skipRawData(blockDescOffset);
    read >> blockOffset;
    read >> blockLength;
#ifdef DEBUG_OUTPUT
    qDebug() << "Block offset:" << blockOffset;
    qDebug() << "Block length:" << blockLength;
#endif

    //Step 7 - find element row and column in block
    elementRow = imageRow - blockRow * Image.blockHeight;
    elementCol = imageCol - blockCol * Image.blockWidth;
#ifdef DEBUG_OUTPUT
    qDebug() << "Element Row:" << elementRow << "Col:" << elementCol ;
#endif

    //Step 8 - find element offset in block
    elementOffset = (elementRow * Image.blockWidth + elementCol) * (Image.elementSize/8);
#ifdef DEBUG_OUTPUT
    qDebug() << "Element Offset:" << elementOffset;
#endif

    if (elementOffset > blockLength) {
        qDebug() << "PING";
    }

    //Step 9 - read height of element
    mtwFile.seek(blockOffset + elementOffset);

    int32_t height;
#ifdef DEBUG_OUTPUT
    qDebug() << "Summ offset " << (blockOffset + elementOffset);
#endif
    read >> height;
    height = height;
#ifdef DEBUG_OUTPUT
    qDebug() << height;
#endif

    if ( (height < Image.minHeight) || (height > Image.maxHeight) ) {
        return -1;
    }

    return height;
}

float Parser::ReadElementInBlock(uint row, uint col)
{
    if (mtwFile.size() == 0) {
        return 0;
    }


    uint32_t elementOffset;
    int32_t height;

    if ( (row > Block.height) || (col > Block.width) ) {
        return BAD_ELEMENT;
    }

    elementOffset = (row * Block.width + col) * (Image.elementSize/8);
#ifdef DEBUG_OUTPUT
    qDebug() << elementOffset;
#endif

    QDataStream read(Block.data);
    read.setByteOrder(QDataStream::LittleEndian);

    read.skipRawData(elementOffset);
    read >> height;
    height = height * Image.measurementUnits;

    if (height == NA) return Image.minHeight;    // -32767 - value equal to "NO HEIGHT" in matrix

    return height;
}

float Parser::GetHeightByElement(int32_t imageRow, int32_t imageCol)
{
    if (mtwFile.size() == 0) {
        return 0;
    }

    float height;
    uint32_t blockRow,
             blockCol,
             elementRow,
             elementCol;

    blockRow = imageRow / Image.blockHeight;
    blockCol = imageCol / Image.blockWidth;
    if ( (blockRow > Image.blockRowCount) || (blockCol > Image.blockColCount)) {
        return BAD_BLOCK;
    }

    if ( (blockRow != Block.row) || (blockCol != Block.col) ) {
        LoadBlock(blockRow, blockCol);
    }

    elementRow = imageRow - blockRow * Image.blockHeight;
    elementCol = imageCol - blockCol * Image.blockWidth;

    if ( Block.row == Image.blockRowCount ) {
        if (elementRow > Image.blockCutHeight-1) {
            return BAD_ELEMENT;
         }
    }
    if ( Block.col == Image.blockColCount ) {
        if (elementCol > Image.blockCutWidth-1) {
            return BAD_ELEMENT;
         }
    }


    height = ReadElementInBlock(elementRow, elementCol);

    if (height == -32767) return NO_HEIGHT;    // -32767 - value equal to "NO HEIGHT" in matrix

    return height;
}

void Parser::GetlHeightsArray()
{
    QVector< QVector<float> > heightsBlock;
    QVector<float> heightsArray;

    heightsBlock.reserve(Image.rowCount);
    heightsBlock.resize(Image.rowCount);


    for (uint blockRow=0; blockRow < Image.blockRowCount; blockRow++) {
        for (uint blockCol=0; blockCol < Image.blockColCount; blockCol++) {

            //fill heightsArray
            LoadBlock(blockRow, blockCol);

            float height;

            for (uint elementRow=0; elementRow<Block.height; elementRow++) {
                for (uint elementCol=0; elementCol< Block.width; elementCol++) {

                    height = ReadElementInBlock(elementRow, elementCol);
                    heightsBlock[blockRow*Image.blockHeight + elementRow].append(height);

                }
            }
        }
    }

    foreach (QVector<float> vector, heightsBlock) {
        heightsArray += vector;
    }

    emit HeightsArrayReady(heightsArray);
}
