# libapng 

This library allows users to create and read APNG (animated PNG) files. 
Its main two classes are:
- png::Writer
- png::Reader

Class png::Writer allows users add individual frames in form of QImage, QPixmap, file name or raw data (QByteArray) and then store the animation in a file.
Class png::Reader allows users to parse an APNG file and split it into individual frames in form of files, QImages or QPixmaps. 
