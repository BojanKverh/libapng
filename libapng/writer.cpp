#include "writer.h"

#include <QBuffer>
#include <QDateTime>
#include <QFile>
#include <QImage>
#include <QPixmap>
#include <QtEndian>

namespace png {

Writer::Writer() {}

void Writer::append(const QByteArray& rba)
{
  bool bFirst = (m_vIDAT.count() == 0);

  quint32 iOffset = m_cbaSig.size();

  auto optChunk = readChunk(rba, iOffset);
  std::optional<Chunk> chunkFrame;

  while (optChunk.has_value() == true) {
    auto chunk = optChunk.value();
    if ((bFirst == true) && (chunk.m_baName == m_cbaIHDR)) {
      m_chunkIHDR = chunk;
      m_iW        = convert(m_chunkIHDR.m_baContent.mid(0, 4));
      m_iH        = convert(m_chunkIHDR.m_baContent.mid(4, 4));
    } else if (chunk.m_baName == m_cbaIDAT) {
      if (bFirst == true) {
        m_vIDAT << chunk;
      } else {
        if (chunkFrame.has_value() == false) {
          chunkFrame = chunk;
          chunkFrame->m_baName = m_cbaFDAT;
        } else {
          chunkFrame->m_baContent.append(chunk.m_baContent);
          chunkFrame->m_uiLength += chunk.m_uiLength;
        }
      }
    } else if (bFirst == true) {
      m_vOtherChunks << chunk;
    }

    optChunk = readChunk(rba, iOffset);
  }

  if (chunkFrame.has_value() == true) {
    m_vfDAT << chunkFrame.value();
  }
}

void Writer::append(QImage* pImg)
{
  QByteArray ba;
  QBuffer buf(&ba);
  buf.open(QIODevice::WriteOnly);
  pImg->save(&buf, "PNG", 0);

  buf.close();

  append(ba);
}

void Writer::append(QPixmap* pPix)
{
  QByteArray ba;
  QBuffer buf(&ba);
  buf.open(QIODevice::WriteOnly);
  pPix->save(&buf, "PNG", 0);
  buf.close();

  append(ba);
}

void Writer::append(const QString& rqsFile)
{
  QFile f(rqsFile);
  if (f.open(QFile::ReadOnly) == false)
    return;

  auto ba = f.readAll();
  f.close();

  append(ba);
}

bool Writer::exportAPNG(const QString& rqsFile, int iFPS)
{
  QFile f(rqsFile);
  if (writeSignature(f) == false)
    return false;

  writeIHDR(f);
  for (const auto& rOther : m_vOtherChunks)
      writeChunk(f, rOther);

  writeText(f);
  writeACTL(f);
  writeFCTL(f, -1, iFPS);
  writeIDAT(f);
  for (int i = 0; i < m_vfDAT.count(); ++i) {
    writeFCTL(f, i, iFPS);
    writeFDAT(f, i);
  }

  writeIEnd(f);

  return true;
}

void Writer::reset()
{
  m_vIDAT.clear();
  m_vfDAT.clear();
}

int Writer::count() const
{
  return (m_vIDAT.count() > 0? 1 : 0) + m_vfDAT.count();
}

bool Writer::writeSignature(QFile& rF) const
{
  if (rF.open(QFile::WriteOnly) == false)
    return false;

  rF.write(m_cbaSig);
  return true;
}

void Writer::writeIHDR(QFile& rF) const
{
  writeChunk(rF, m_chunkIHDR);
}

void Writer::writeText(QFile& rF) const
{
  Chunk chunk;

  chunk.m_baContent = prepareText("Creation time", QDateTime::currentDateTime().toString("ddd, dd MM yyyy HH:mm:ss"));
  chunk.m_baName = m_cbaTEXT;
  chunk.m_uiLength = chunk.m_baContent.size();
  chunk.m_baCRC = convert(crc(chunk));
  writeChunk(rF, chunk);
  qDebug() << chunk.m_baName << chunk.m_baContent << chunk.m_uiLength << chunk.m_baCRC.toHex();

  chunk.m_baContent = prepareText("Software", "libapng v1.0");
  chunk.m_baName = m_cbaTEXT;
  chunk.m_uiLength = chunk.m_baContent.size();
  chunk.m_baCRC = convert(crc(chunk));
  writeChunk(rF, chunk);
}

void Writer::writeACTL(QFile& rF) const
{
  writeChunk(rF, actl(1 + m_vfDAT.count(), 0));
}

void Writer::writeFCTL(QFile& rF, int i, int iFPS) const
{
  writeChunk(rF, fctl(i, m_iW, m_iH, iFPS, 0, 0));
}

void Writer::writeIDAT(QFile& rF) const
{
  for (const auto& rIDAT : m_vIDAT) {
    writeChunk(rF, rIDAT);
  }
}

void Writer::writeFDAT(QFile& rF, int i) const
{
  Chunk chunk = m_vfDAT[i];
  chunk.m_baContent.prepend(convert(2 * i + 2));
  chunk.m_uiLength += 4;
  writeChunk(rF, chunk, true);
}

void Writer::writeIEnd(QFile& rF) const
{
  writeChunk(rF, iend());
  rF.close();
}

QByteArray Writer::prepareText(const QString& rqsKey, const QString& rqsValue) const
{
  auto ba = rqsKey.toLatin1();
  QByteArray baZero = QByteArray::fromHex("00");
  ba.append(baZero);
  ba.append(rqsValue.toLatin1());
  return ba;
}

}
