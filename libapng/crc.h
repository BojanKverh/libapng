#pragma once

#include <QtGlobal>

class QByteArray;

namespace png {
/**
 * @brief The CRC class This class is used to calculate CRC32
 */
class __declspec(dllexport) CRC
{
public:
  /**
   * @brief CRC Default constructor. Builds the internal table
   */
  CRC();
  /**
   * @brief calculate Calculates the CRC32 of a given byte array
   * @param rba Byte array to calculate CRC32 from
   * @return Calculated value
   */
  quint32 calculate(const QByteArray& rba) const;

private:
  quint32 m_crcTable[256];
};

}
