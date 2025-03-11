#include <QImage>
#include <QPainter>
#include <QTemporaryFile>
#include <QtTest>

// add necessary includes here
#include "../libapng/crc.h"
#include "../libapng/reader.h"
#include "../libapng/writer.h"

class TestLibApng : public QObject
{
  Q_OBJECT

public:
  TestLibApng();
  ~TestLibApng();

private:
  QImage prepareImage(int i) const;

private slots:
  void initTestCase();
  void cleanupTestCase();

  void crcOutput_data();
  void crcOutput();

  void writerBinaryTest();
  void readerWriterTest();
};

TestLibApng::TestLibApng() {}

TestLibApng::~TestLibApng() {}

QImage TestLibApng::prepareImage(int i) const
{
  QImage img(100, 100, QImage::Format_ARGB32);
  img.fill(Qt::transparent);
  QPainter P(&img);
  P.setPen(Qt::red);
  P.setBrush(Qt::red);
  P.drawRoundedRect(i * 10, i * 10, 10, 10, 3, 3);

  return img;
}

void TestLibApng::initTestCase() {}

void TestLibApng::cleanupTestCase() {}

void TestLibApng::crcOutput_data()
{
  QTest::addColumn<QString>("data");
  QTest::addColumn<quint32>("value");

  QTest::newRow("empty") << "" << 0x00000000U;
  QTest::newRow("IEND") << "49454E44" << 0xAE426082U;
  QTest::newRow("IHDR") << "4948445200000258000002580806000000" << 0xBE6698DCU;
  QTest::newRow("ACTL") << "6163544C0000007800000000" << 0x40EF6B1EU;
  QTest::newRow("FCTL") << "6663544C0000000000000258000002580000000000000000002103E80000"
                        << 0x16E15EB9U;
  QString qsText = QString("tEXtCreation time%1Tue, 11 03 2025 14:36:48")
                     .arg(QByteArray::fromHex("00"))
                     .toLatin1()
                     .toHex();
  QTest::newRow("tEXt") << qsText << 0x8F5835ADU;
}

void TestLibApng::crcOutput()
{
  png::CRC crc;
  QFETCH(QString, data);
  QFETCH(quint32, value);

  auto bytes = QByteArray::fromHex(data.toLatin1());
  QCOMPARE(crc.calculate(bytes), value);
}

void TestLibApng::writerBinaryTest()
{
  png::Writer writerImg;
  png::Writer writerPix;
  png::Writer writerFile;

  for (int i = 0; i < 10; ++i) {
    auto img = prepareImage(i);
    writerImg.append(&img);
    auto pix = QPixmap::fromImage(img);
    writerPix.append(&pix);
    QTemporaryFile tfFile;
    tfFile.open();
    tfFile.close();
    img.save(tfFile.fileName(), "PNG", 0);
    writerFile.append(tfFile.fileName());
  }

  QTemporaryFile tfImg;
  tfImg.open();
  tfImg.close();
  writerImg.exportAPNG(tfImg.fileName(), 30);
  QTemporaryFile tfPix;
  tfPix.open();
  tfPix.close();
  writerPix.exportAPNG(tfPix.fileName(), 30);
  QTemporaryFile tfFiles;
  tfFiles.open();
  tfFiles.close();
  writerFile.exportAPNG(tfFiles.fileName(), 30);

  QFile fImg(tfImg.fileName());
  QFile fPix(tfPix.fileName());
  QFile fFile(tfFiles.fileName());

  fImg.open(QFile::ReadOnly);
  auto baImg = fImg.readAll();
  fImg.close();
  fPix.open(QFile::ReadOnly);
  auto baPix = fPix.readAll();
  fPix.close();
  fFile.open(QFile::ReadOnly);
  auto baFile = fFile.readAll();
  fFile.close();

  // these tests might occasionally fail, because the APNG files also store Creation time, which
  // can be different for two different files, created one after another. But this is a rare
  // occasion and should not be a reason for concern unless these tests start failing regularly.
  QCOMPARE(baImg.size(), baPix.size());
  QCOMPARE(baImg.size(), baFile.size());
  QCOMPARE(baImg, baPix);
  QCOMPARE(baImg, baFile);
}

void TestLibApng::readerWriterTest()
{
  png::Writer writer;
  png::Reader reader;

  QVector<QImage> vImg1;
  for (int i = 0; i < 10; ++i) {
    vImg1 << prepareImage(i);
    writer.append(&vImg1.last());
  }

  QTemporaryFile tf;
  tf.open();
  tf.close();
  writer.exportAPNG(tf.fileName(), 30);

  auto vImg2 = reader.importImages(tf.fileName());

  QCOMPARE(vImg1.count(), vImg2.count());
  for (int i = 0; i < qMin(vImg1.count(), vImg2.count()); ++i) {
    QString qsErrMsg = "Written and read images sizes are not the same (index %1)";
    QVERIFY2(vImg1[i].size() == vImg2[i].size(), qsErrMsg.arg(i).toLatin1());
    qsErrMsg = "Written and read images do not have the same content (index %1)";
    QVERIFY2(vImg1[i] == vImg2[i], qsErrMsg.arg(i).toLatin1());
  }
}

QTEST_MAIN(TestLibApng)

#include "tst_libapng.moc"
