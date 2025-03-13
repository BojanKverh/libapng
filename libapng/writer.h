#pragma once

#include <QVector>

#include "base.h"

class QImage;
class QPixmap;
class QFile;

namespace png {

/**
 * @brief The Writer class This class can be used to export a collection of images into APNG
 * animation. Typical usage of this class consists of several append calls, with which
 * the individual frames are stored in the object's internal container, and one final exportAPNG
 * call, which effectively creates the animated PNG from the stored frames and stores it in a file.
 */
class __declspec(dllexport) Writer : public Base
{
public:
  /**
   * @brief Writer Default constructor
   */
  Writer();

  /**
   * @brief append Adds the image, stored in the byte array, to include in the animation
   * @param rba Reference to the byte array, containing the image data. Image data should contain a valid PNG image
   */
  void append(const QByteArray& rba);
  /**
   * @brief append Adds an image to include in the animation
   * @param pImg Pointer to the image to include
   */
  void append(QImage* pImg);
  /**
   * @brief append Adds a pixmap to include in the animation
   * @param pPix Pointer to the pixmap to include
   */
  void append(QPixmap* pPix);
  /**
   * @brief append Adds an image, read from the given file, to include in the animation
   * @param rqsFile Path to a file to include
   */
  void append(const QString& rqsFile);
  /**
   * @brief exportAPNG Exports the included images to APNG file
   * @param rqsFile Full path to the file to write the animation to
   * @param iFPS Frames per second value
   * @return true on success and false on failure
   */
  bool exportAPNG(const QString& rqsFile, int iFPS);
  /**
   * @brief reset This method removes all the stored images from the object's container, making it
   * possible to reuse objects of this class to create more than one animated PNG
   */
  void reset() override;
  /**
   * @brief count Returns the number of frames stored in the object's container
   * @return number of frames stored in the object's container
   */
  int count() const;

private:
  /**
   * @brief writeSignature Writes the PNG signature into given file
   * @param rF Reference to file to write into
   * @return true on success and false on failure
   */
  bool writeSignature(QFile& rF) const;
  /**
   * @brief writeIHDR Writes the IHDR chunk into file
   * @param rF Reference to file to write into
   */
  void writeIHDR(QFile& rF) const;
  /**
   * @brief writeText Writes the textual data
   * @param rF
   */
  void writeText(QFile& rF) const;
  /**
   * @brief writeACTL Writes the ACTL chunk
   * @param rF Reference to file to write into
   */
  void writeACTL(QFile& rF) const;
  /**
   * @brief writeFCTL Writes the FCTL chunk
   * @param rF Reference to file to write into
   * @param i Index of the image
   * @param iFPS Frame per second rate
   */
  void writeFCTL(QFile& rF, int i, int iFPS) const;
  /**
   * @brief writeIDAT Writes the IDAT chunks
   * @param rF Reference to file to write into
   */
  void writeIDAT(QFile& rF) const;
  /**
   * @brief writeFDAT Writes the i-th fdAT chunk
   * @param rF Reference to file to write into
   * @param i fdAT chunk index
   */
  void writeFDAT(QFile& rF, int i) const;
  /**
   * @brief writeIEnd Writes the IEND chunk and closes the file
   * @param rF Reference to file to write into
   */
  void writeIEnd(QFile& rF) const;
  /**
   * @brief prepareText Prepares the text for storing into tEXt chunk
   * @param rqsKey Text key
   * @param rqsValue Text value
   * @return
   */
  QByteArray prepareText(const QString& rqsKey, const QString& rqsValue) const;

private:
  QVector<Chunk> m_vIDAT;
  QVector<Chunk> m_vfDAT;
  QVector<Chunk> m_vOtherChunks;
  Chunk m_chunkIHDR;
  int m_iW;
  int m_iH;

};

}
