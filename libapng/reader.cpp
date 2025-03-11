#include "reader.h"

#include <QBuffer>
#include <QFile>

namespace png {

Reader::Reader() {}

QVector<QByteArray> Reader::import(const QString& rqsFile)
{
  QVector<QByteArray> vImg;

  auto ba = readContent(rqsFile);
  if (ba.size() > 0) {
    parseChunks(ba);

    auto vIDAT = split(m_baIDAT);

    auto baContent = m_cbaSig;
    writeChunk(baContent, m_chunkIHDR);
    for (const auto& rOther : m_vOtherChunks) {
      writeChunk(baContent, rOther);
    }
    for (const auto& rba : vIDAT) {
      baContent.append(rba);
    }

    writeChunk(baContent, iend());
    vImg << baContent;

    for (const auto& rFDAT : m_vfDAT) {
      baContent = m_cbaSig;
      writeChunk(baContent, m_chunkIHDR);
      for (const auto& rOther : m_vOtherChunks)
        writeChunk(baContent, rOther);
      auto vIDAT = split(rFDAT.m_baContent.mid(4));
      for (const auto& rba : vIDAT) {
        baContent.append(rba);
      }
      writeChunk(baContent, iend());
      vImg << baContent;
    }
  }

  return vImg;
}

void Reader::import(const QString& rqsFile, const QString& rqsOutFile)
{
  auto vbaContent = import(rqsFile);

  int iLen = qFloor(qLn((1 + vbaContent.count()) / qLn(10)));

  int i = 0;
  for (const auto& rba : vbaContent) {
    auto qsFile = rqsOutFile.arg(QString::number(i++).rightJustified(iLen, '0'));
    QFile f(qsFile);
    f.open(QFile::WriteOnly);
    f.write(rba);
    f.close();
  }
}

QVector<QImage> Reader::importImages(const QString& rqsFile)
{
  auto vbaContent = import(rqsFile);
  QVector<QImage> vImg;

  for (auto& rba : vbaContent) {
    QBuffer buf(&rba);
    buf.open(QIODevice::ReadOnly);
    buf.seek(0);
    QImage img;
    img.load(&buf, "png");
    buf.close();
    vImg << img;
  }

  return vImg;
}

QVector<QPixmap> Reader::importPixmaps(const QString& rqsFile)
{
  auto vbaContent = import(rqsFile);
  QVector<QPixmap> vPix;

  for (auto& rba : vbaContent) {
    QPixmap pix;
    pix.loadFromData(rba, "PNG");
    vPix << pix;
  }

  return vPix;
}

void Reader::reset()
{
  m_chunkIHDR = Chunk();
  m_baIDAT.clear();
  m_vfDAT.clear();
  m_vOtherChunks.clear();
}

QByteArray Reader::readContent(const QString& rqsFile) const
{
  QByteArray ba;
  QFile f(rqsFile);
  if (f.open(QFile::ReadOnly) == true) {
    ba = f.readAll();
    f.close();
  }

  // if the signature is not found, clear the content, which signals an error
  if (ba.left(8) != m_cbaSig)
    ba.clear();

  return ba;
}

void Reader::parseChunks(const QByteArray& rba)
{
  quint32 uiOffset = m_cbaSig.size();
  parseIHDR(rba, uiOffset);

  auto optChunk = readChunk(rba, uiOffset);
  while (optChunk.has_value() == true) {
    auto chunk = optChunk.value();
    if (chunk.m_baName == m_cbaIDAT)
      m_baIDAT.append(chunk.m_baContent);
    else if (chunk.m_baName == m_cbaFDAT)
      m_vfDAT << chunk;
    else if ((chunk.m_baName != m_cbaACTL) && (chunk.m_baName != m_cbaFCTL)
             && (chunk.m_baName != m_cbaIEND))
      m_vOtherChunks << chunk;

    optChunk = readChunk(rba, uiOffset);
  }
}

void Reader::parseIHDR(const QByteArray& rba, quint32& riOffset)
{
  auto opt = readChunk(rba, riOffset);
  if (opt.has_value() == true)
    m_chunkIHDR = opt.value();
}

QByteArray Reader::parseIDAT(const QByteArray& rba) const
{
  QByteArray baIDAT;
  auto ind = rba.indexOf(m_cbaIDAT);
  while (ind >= 4) {
    auto uiLength = convert(rba.mid(ind - 4, 4));
    if (ind + uiLength + 8 <= rba.size()) {
      baIDAT.append(rba.mid(ind + 4, uiLength));
    }
    ind = rba.indexOf(m_cbaIDAT, ind + uiLength + 8);
  }
  return baIDAT;
}

QPair<QByteArray, int> Reader::parseFDAT(const QByteArray& rba, int iOffset) const
{
  QPair<QByteArray, int> result;
  result.second = -1;

  auto ind = rba.indexOf(m_cbaFDAT, iOffset);
  if (ind >= 0) {
    auto uiLength = convert(rba.mid(ind - 4, 4));
    result.first  = rba.mid(ind + 8, uiLength - 4);
    result.second = ind + uiLength + 4;
  }
  return result;
}

QVector<QByteArray> Reader::split(const QByteArray& rba) const
{
  QVector<QByteArray> vIDAT;
  for (quint32 ui = 0; ui < rba.size(); ui += m_cuiLibPngLimit) {
    QByteArray ba = rba.mid(ui, m_cuiLibPngLimit);
    ba.prepend(m_cbaIDAT);
    auto uiCrc = m_crc.calculate(ba);
    ba.prepend(convert(ba.size() - 4));
    ba.append(convert(uiCrc));
    vIDAT << ba;
  }

  return vIDAT;
}

} // namespace png
