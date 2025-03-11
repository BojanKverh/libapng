#include "crc.h"

#include <QByteArray>
namespace png {

CRC::CRC()
{
  for (quint32 i = 0; i < 256; i++) {
    quint32 crc = i;
    for (quint32 j = 0; j < 8; j++) {
      if (crc & 1) {
        crc = (crc >> 1) ^ 0xEDB88320;
      } else {
        crc >>= 1;
      }
    }
    m_crcTable[i] = crc;
  }
}

quint32 CRC::calculate(const QByteArray& rba) const
{
  quint32 crc = 0xFFFFFFFF;
  for (unsigned char byte : rba) {
    crc = (crc >> 8) ^ m_crcTable[(crc & 0xFF) ^ byte];
  }
  return crc ^ 0xFFFFFFFF;
}

}
