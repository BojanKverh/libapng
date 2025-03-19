#include "windowmain.h"

#include "widgetapng.h"

#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QLayout>

WindowMain::WindowMain(QWidget *parent)
    : QMainWindow(parent)
{
  m_pwCentral = new WidgetApng(this);
  setCentralWidget(m_pwCentral);

  auto* pMenuBar = new QMenuBar;
  auto* pMenu = new QMenu("File");

  pMenu->addAction("Open APNG", [this](){
    auto qsFile = QFileDialog::getOpenFileName(this, "Select APNG", ".", "*.png");
    if (qsFile.isEmpty() == false)
      m_pwCentral->load(qsFile);
  }
    );

  pMenu->addAction("Quit", QApplication::instance(), &QApplication::quit);
  pMenuBar->addMenu(pMenu);
  setMenuBar(pMenuBar);

  connect(m_pwCentral, &WidgetApng::resizeSignal, [this](const QSize& size) {
      resize(size.width(), size.height() + menuBar()->height());
  });
}

WindowMain::~WindowMain() {}
