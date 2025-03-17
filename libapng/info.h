#pragma once

#include <QtGlobal>
#include <QString>
#include <QDebug>

namespace png {

/**
 * @brief The Info class This class provides info about the parsed PNG/APNG file
 */
class __declspec(dllexport) Info
{
public:
  /**
   * @brief The Type enum Denotes the PNG types
   */
  enum class Type { etInvalid, etPNG, etAPNG };
  /**
   * @brief The ParseError enum This enum denotes the type of parsing error encountered
   */
  enum class ParseError {
    epeNone,
    epeNoSignature,
    epeNoIHDR,
    epeNoIDAT,
    epeNoIEND,
    epeNoACTL,
    epeCRC,
    epeChunkName,
    epeInvalidSize
  };

  /**
   * @brief Info Default constructor
   */
  Info();
  /**
   * @brief isOk Returns true, if this object contains no parse error code
   * @return true, if this object contains no parse error and false otherwise
   */
  bool isOk() const { return m_epeError == ParseError::epeNone; }
  /**
   * @brief type Returns the PNG file type
   * @return PNG file type
   */
  Type type() const { return m_eType; }
  /**
   * @brief fps Returns frames per second rate
   * @return frames per second rate
   */
  quint32 fps() const { return m_uiFPS; }
  /**
   * @brief framesCount Returns the number of frames
   * @return number of frames
   */
  quint32 framesCount() const { return m_uiFrames; }
  /**
   * @brief error Returns the error
   * @return Error code
   */
  ParseError error() const { return m_epeError; }
  /**
   * @brief errorMessage Returns the error message
   * @return Error message
   */
  QString errorMessage() const { return m_qsErrorMsg; }
  /**
   * @brief offset Returns the offset of the error
   * @return Error offset in [bytes]
   */
  quint32 offset() const { return m_uiOffset; }
  /**
   * @brief setType Sets the type
   * @param eType New PNG file type
   */
  void setType(Type eType) { m_eType = eType; }
  /**
   * @brief setFPS Sets the frames per second rate
   * @param uiFPS New frames per second rate
   */
  void setFPS(quint32 uiFPS) { m_uiFPS = uiFPS; }
  /**
   * @brief setFrameCount Sets the new frame count
   * @param uiFrames New frame count
   */
  void setFrameCount(quint32 uiFrames) { m_uiFrames = uiFrames; }

  /**
   * @brief setError Sets the parsing error parameters
   * @param epeError Error type
   * @param rqsMsg Error message
   * @param uiOffset Offset, where the error occured
   */
  void setError(ParseError epeError, const QString& rqsMsg, quint32 uiOffset);
  /**
   * @brief reset Resets the object attributes to their default values
   */
  void reset();

private:
  Type m_eType;
  quint32 m_uiFPS;
  quint32 m_uiFrames;
  ParseError m_epeError;
  QString m_qsErrorMsg;
  quint32 m_uiOffset;
};

__declspec(dllexport) QDebug operator<<(QDebug dbg, const Info &info);


} // namespace png
