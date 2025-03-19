#pragma once

#include <QWidget>
#include <QVector>
#include <QImage>
#include <QTimer>

class QPaintEvent;

/**
 * @brief The WidgetApng class This widget is used to display APNG images
 */
class WidgetApng : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief WidgetApng Constructor
   * @param pParent Pointer to the parent widget
   */
  WidgetApng(QWidget* pParent = nullptr);
  /**
   * @brief load Loads the APNG from given file
   * @param rqsFile File to load the APNG from
   */
  void load(const QString& rqsFile);
  /**
   * @brief sizeHint Returns the ideal size
   * @return Ideal size
   */
  QSize sizeHint() const override;

signals:
  /**
   * @brief resizeSignal Emitted when the widget is resized by load call
   * @param size New size
   */
  void resizeSignal(const QSize& size);

protected:
  /**
   * @brief paintEvent Used to draw one of the frames
   * @param pEvent Pointer to the paint event object
   */
  void paintEvent(QPaintEvent* pEvent) override;

private:
  QVector<QImage> m_vFrames;
  QTimer m_timer;

  quint32 m_uiCurrent;
};
