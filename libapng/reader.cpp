#include "reader.h"

#include <QBuffer>
#include <QFile>

namespace png {

Reader::Reader() {}

QVector<QByteArray> Reader::import(const QString& rqsFile)
{
  reset();
  // default type is PNG
  m_info.setType(Info::Type::etPNG);

  QVector<QByteArray> vImg;

  auto ba = readContent(rqsFile);
  if (ba.size() > 0 && m_info.error() == Info::ParseError::epeNone) {
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
    img.load(&buf, "PNG");
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
  Base::reset();
  m_chunkIHDR = Chunk();
  m_baIDAT.clear();
  m_vfDAT.clear();
  m_vOtherChunks.clear();
}

QByteArray Reader::readContent(const QString& rqsFile)
{
  QByteArray ba;
  QFile f(rqsFile);
  if (f.open(QFile::ReadOnly) == true) {
    ba = f.readAll();
    f.close();
  }

  // if the signature is not found, clear the content, which signals an error
  if (ba.left(8) != m_cbaSig) {
    m_info.setType(Info::Type::etInvalid);
    m_info.setError(Info::ParseError::epeNoSignature,
                    "No PNG signature found at the beginning of the file", 0U);
  }

  return ba;
}

void Reader::parseChunks(const QByteArray& rba)
{
  bool bIEND       = false;
  bool bACTL       = false;
  quint32 uiOffset = m_cbaSig.size();
  parseIHDR(rba, uiOffset);
  if (m_info.isOk() == false)
    return;

  auto optChunk = readChunk(rba, uiOffset);
  while (optChunk.has_value() == true) {
    auto chunk = optChunk.value();
    if (chunk.m_baName == m_cbaIDAT)
      m_baIDAT.append(chunk.m_baContent);
    else if (chunk.m_baName == m_cbaFDAT)
      m_vfDAT << chunk;
    else if (chunk.m_baName == m_cbaIEND)
      bIEND = true;
    else if ((chunk.m_baName != m_cbaACTL) && (chunk.m_baName != m_cbaFCTL))
      m_vOtherChunks << chunk;

    if (chunk.m_baName == m_cbaFCTL) {
      auto num   = convert(chunk.m_baContent.mid(20, 2));
      auto denom = convert(chunk.m_baContent.mid(22, 2));
      m_info.setFPS(num > 0 ? denom / num : 0);
    }

    if (chunk.m_baName == m_cbaACTL) {
      bACTL = true;
    }

    optChunk = readChunk(rba, uiOffset);
  }

  m_info.setFrameCount((m_baIDAT.size() > 0 ? 1 : 0) + m_vfDAT.count());

  if (m_info.isOk() == false)
    return;

  if (m_baIDAT.size() == 0) {
    m_info.setError(Info::ParseError::epeNoIDAT, "No IDAT chunk found", uiOffset);
    return;
  }

  if ((m_info.type() == Info::Type::etAPNG) && (bACTL == false)) {
    m_info.setError(Info::ParseError::epeNoACTL, "No ACTL chunk found", uiOffset);
    return;
  }

  if (bIEND == false) {
    m_info.setError(Info::ParseError::epeNoIEND, "No IEND chunk found ", uiOffset);
  }
}

void Reader::parseIHDR(const QByteArray& rba, quint32& riOffset)
{
  auto opt = readChunk(rba, riOffset);
  if (opt.has_value() == true)
    m_chunkIHDR = opt.value();

  if (m_chunkIHDR.m_baName != m_cbaIHDR) {
    m_info.setError(Info::ParseError::epeNoIHDR, "No IHDR chunk found", riOffset);
  }
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
