#pragma once

#include "base.h"

#include <QByteArray>
#include <QImage>
#include <QPixmap>
#include <QVector>

namespace png {

/**
 * @brief The Reader class This class can be used to split the animated PNG back into a sequence of individual frames. This class contains a number
 * of methods, which take an APNG file name as parameter and split it into a vector of files, QImages or QPixmaps.
 */
class __declspec(dllexport) Reader : public Base
{
public:
    /**
     * @brief Reader Default constructor
     */
    Reader();
    /**
     * @brief import Reads the APNG file and splits it into individual frames. If an error occured during APNG parsing, this method will return an
     * empty vector. In any case, the caller should call the info().isOk() method to determine, whether the file was parsed correctly or not.
     * @param rqsFile Full path to the file to read
     * @return Imported frames in a vector of binary content
     */
    QVector<QByteArray> import(const QString& rqsFile);
    /**
     * @brief import Reads the APNG file and splits it into individual frames
     * @param rqsFile Full path to the file to read
     * @param rqsOutFile This string should contain an output file name template. Individual frames
     * will be stored into rqsOutFile, where the template string "%1" will be replaced by the frame number
     */
    void import(const QString& rqsFile, const QString& rqsOutFile);

    /**
     * @brief importImages Reads the APNG file and splits it into individual frames
     * @param rqsFile Full path to the file to read
     * @return Imported frames in a vector of QImages
     */
    QVector<QImage> importImages(const QString& rqsFile);

    /**
     * @brief importPixmaps Reads the APNG file and splits it into individual frames
     * @param rqsFile Full path to the file to read
     * @return Imported frames in a vector of QPixmaps
     */
    QVector<QPixmap> importPixmaps(const QString& rqsFile);
    /**
     * @brief reset Resets all the parsed data. This method is called automatically by all the import
     * methods, so no need to call it explicitly, unless the resources taken by the individual frames
     * need to be released.
     */
    void reset() override;

private:
    /**
     * @brief readContent Reads the content of the file and returns it
     * @param rqsFile Full path to the file to read
     * @return Content of the file
     */
    QByteArray readContent(const QString& rqsFile);
    /**
     * @brief parseChunks Parses PNG chunks
     * @param rba Reference to the byte array to parse
     */
    void parseChunks(const QByteArray& rba);
    /**
     * @brief parseIHDR parses the IHDR chunk
     * @param rba APNG content
     * @param riOffset reference to the offset variable
     */
    void parseIHDR(const QByteArray& rba, quint32& riOffset);
    /**
     * @brief parseFDAT Parses the content to find a fdAT chunk
     * @param rba PNG content
     * @param iOffset Offset to start the search
     * @return FDAT content (excluding size, fdAT, index and CRC) and position of the found chunk
     */
    QPair<QByteArray, int> parseFDAT(const QByteArray& rba, int iOffset = 0) const;
    /**
     * @brief split Splits the content into chunks of 8192 bytes (png library limit). It also equips the chunks with size, IDAT and CRC
     * @param rba Reference to the content to split
     * @return Vector of IDAT chunks, ready to be written to file
     */
    QVector<QByteArray> split(const QByteArray& rba) const;

private:
    Chunk m_chunkIHDR;
    QByteArray m_baIDAT;
    QVector<Chunk> m_vfDAT;
    QVector<Chunk> m_vOtherChunks;
};

}
