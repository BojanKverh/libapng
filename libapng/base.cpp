#include "base.h"

#include <QFile>

namespace png {

QByteArray Base::convert(quint32 uiValue) const
{
  QByteArray ba;
  ba.append(char(uiValue >> 24));
  ba.append(char((uiValue >> 16) & 0xff));
  ba.append(char((uiValue >> 8) & 0xff));
  ba.append(char(uiValue & 0xff));
  return ba;
}

quint32 Base::convert(const QByteArray& rba) const
{
  quint32 uiVal;
  uiVal = ((unsigned char) rba[0]) << 24;
  uiVal += ((unsigned char) rba[1]) << 16;
  uiVal += (((unsigned char) rba[2]) << 8);
  uiVal += (unsigned char) rba[3];
  return uiVal;
}

quint32 Base::crc(const Chunk& rChunk) const
{
  QByteArray ba = rChunk.m_baName;
  ba.append(rChunk.m_baContent);
  return m_crc.calculate(ba);
}

std::optional<Chunk> Base::readChunk(const QByteArray& rba, quint32& riOffset) const
{
  if (riOffset + 12 >= rba.size()) {
    return {};
  }

  Chunk chunk;
  chunk.m_uiLength = convert(rba.mid(riOffset, 4));
  chunk.m_baName = rba.mid(riOffset + 4, 4);
  chunk.m_baContent = rba.mid(riOffset + 8, chunk.m_uiLength);
  chunk.m_baCRC = rba.mid(riOffset + 8 + chunk.m_uiLength, 4);

  riOffset += chunk.size();

  return chunk;
}

void Base::writeChunk(QByteArray& rba, const Chunk& rChunk, bool bCalcCRC) const
{
  QByteArray baCRC = (bCalcCRC == true? convert(crc(rChunk)) : rChunk.m_baCRC);

  rba.append(convert(rChunk.m_uiLength));
  rba.append(rChunk.m_baName);
  rba.append(rChunk.m_baContent);
  rba.append(baCRC);
}

void Base::writeChunk(QFile& rF, const Chunk& rChunk, bool bCalcCRC) const
{
  QByteArray ba;
  writeChunk(ba, rChunk, bCalcCRC);
  rF.write(ba);
}

Chunk Base::actl(quint32 iCount, quint32 iRepeat) const
{
  Chunk chunk;
  chunk.m_baName = m_cbaACTL;
  chunk.m_baContent.append(convert(iCount));
  chunk.m_baContent.append(convert(iRepeat));

  chunk.m_uiLength = chunk.m_baContent.size();
  chunk.m_baCRC = convert(crc(chunk));
  return chunk;
}

Chunk Base::fctl(int i, int iW, int iH, int iFPS, int iX, int iY, int iDispose, int iBlend) const
{
  Chunk chunk;
  chunk.m_baName = m_cbaFCTL;
  chunk.m_baContent = convert(i >= 0 ? 2 * i + 1 : 0);
  chunk.m_baContent.append(convert(iW));
  chunk.m_baContent.append(convert(iH));
  chunk.m_baContent.append(convert(iX));
  chunk.m_baContent.append(convert(iY));
  chunk.m_baContent.append(convert(1000 / iFPS).right(2));
  chunk.m_baContent.append(convert(1000).right(2));
  chunk.m_baContent.append(convert(iDispose).right(1));
  chunk.m_baContent.append(convert(iBlend).right(1));

  chunk.m_uiLength = chunk.m_baContent.size();
  chunk.m_baCRC = convert(crc(chunk));
  return chunk;
}

Chunk Base::iend() const
{
  Chunk chunk;
  chunk.m_uiLength = 0U;
  chunk.m_baName = m_cbaIEND;

  chunk.m_uiLength = chunk.m_baContent.size();
  chunk.m_baCRC = convert(crc(chunk));
  return chunk;
}

}
