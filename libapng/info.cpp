#include "info.h"

namespace png {

Info::Info() { reset(); }

void Info::setError(ParseError epeError, const QString& rqsMsg, quint32 uiOffset)
{
    m_epeError   = epeError;
    m_qsErrorMsg = rqsMsg;
    m_uiOffset   = uiOffset;
}

void Info::reset()
{
  m_eType = Type::etInvalid;
  m_uiFPS = 0U;
  m_uiFrames = 0U;
  m_epeError = ParseError::epeNone;
  m_qsErrorMsg.clear();
  m_uiOffset = 0U;
}

QDebug operator<<(QDebug dbg, const Info& info)
{
    dbg.nospace() << "PNG Info" << Qt::endl;
    dbg.nospace() << "Type: " << (info.type() == Info::Type::etInvalid? "Invalid" :
                                      (info.type() == Info::Type::etPNG? "PNG" : "APNG"))
                                  << Qt::endl;

    dbg.nospace() << "Frames: " << info.framesCount() << Qt::endl;

    if (info.type() == Info::Type::etAPNG) {
        dbg.nospace() << "FPS: " << info.fps() << Qt::endl;
    }

    if (info.error() != Info::ParseError::epeNone) {
        dbg.nospace() << "PARSE ERROR " << info.errorMessage() << " (0x"
                      << QString::number(info.offset(), 16).toUtf8().constData() << ")" << Qt::endl;
    }

    return dbg.space();
}

}
