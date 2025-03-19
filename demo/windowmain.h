#pragma once

#include <QMainWindow>

class WidgetApng;

class WindowMain : public QMainWindow
{
    Q_OBJECT

public:
  WindowMain(QWidget *parent = nullptr);
  ~WindowMain();

private:
  WidgetApng* m_pwCentral;
};
