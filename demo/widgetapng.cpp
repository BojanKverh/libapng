#include "widgetapng.h"

#include "reader.h"

#include <QPaintEvent>
#include <QPainter>

WidgetApng::WidgetApng(QWidget* pParent) : QWidget(pParent)
{
    connect(&m_timer, &QTimer::timeout, [&]() {
        update();
        if (m_vFrames.count() > 0) {
            m_uiCurrent = (++m_uiCurrent) % m_vFrames.count();
        }
    });
}

void WidgetApng::load(const QString& rqsFile)
{
  png::Reader reader;
  m_vFrames = reader.importImages(rqsFile);
  m_uiCurrent = 0;
  if (reader.info().fps() > 0)
    m_timer.start(1000 / reader.info().fps());

  if (m_vFrames.count() > 0) {
    resize(sizeHint());
    emit resizeSignal(sizeHint());
  }
}

QSize WidgetApng::sizeHint() const
{
  if (m_uiCurrent < m_vFrames.count())
    return m_vFrames[m_uiCurrent].size();
  else
    return QSize(320, 240);
}

void WidgetApng::paintEvent(QPaintEvent* pEvent)
{
  if (m_uiCurrent < m_vFrames.count()) {
    QPainter P(this);
    P.drawImage(0, 0, m_vFrames[m_uiCurrent]);
  }
}
