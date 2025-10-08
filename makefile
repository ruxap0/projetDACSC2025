#Makefile for the DACSC Project

.SILENT:

PROTOCOLSPATH = ./protocols/
CLIENTPATH = ./ClientConsultationBookerQt/
OBJECTSPATH = ./objects/

OBJECTS = $(OBJECTSPATH)lib_socket.o $(OBJECTSPATH)mainclient.o $(OBJECTSPATH)mainwindowclientconsultationbooker.o $(OBJECTSPATH)moc_mainwindowclientconsultationbooker.o $(OBJECTSPATH)CBP.o $(OBJECTSPATH)Serveur.o
PROGRAMS = ClientWindow Serveur

all: $(PROGRAMS)

ClientWindow: $(OBJECTSPATH)mainclient.o $(OBJECTSPATH)mainwindowclientconsultationbooker.o $(OBJECTSPATH)moc_mainwindowclientconsultationbooker.o $(OBJECTSPATH)lib_socket.o
	g++ -Wno-unused-parameter -o ClientWindow $(OBJECTSPATH)mainclient.o $(OBJECTSPATH)mainwindowclientconsultationbooker.o $(OBJECTSPATH)moc_mainwindowclientconsultationbooker.o $(OBJECTSPATH)lib_socket.o /usr/lib64/libQt5Widgets.so /usr/lib64/libQt5Gui.so /usr/lib64/libQt5Core.so /usr/lib64/libGL.so -lpthread

Serveur: $(OBJECTSPATH)Serveur.o $(OBJECTSPATH)lib_socket.o $(OBJECTSPATH)CBP.o
	g++ -Wno-unused-parameter -o Serveur $(OBJECTSPATH)Serveur.o $(OBJECTSPATH)lib_socket.o $(OBJECTSPATH)CBP.o -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient -lpthread

$(OBJECTSPATH)mainclient.o: $(CLIENTPATH)main.cpp
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o $(OBJECTSPATH)mainclient.o $(CLIENTPATH)main.cpp

$(OBJECTSPATH)mainwindowclientconsultationbooker.o: $(CLIENTPATH)mainwindowclientconsultationbooker.cpp
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o $(OBJECTSPATH)mainwindowclientconsultationbooker.o $(CLIENTPATH)mainwindowclientconsultationbooker.cpp

$(OBJECTSPATH)moc_mainwindowclientconsultationbooker.o: $(CLIENTPATH)moc_mainwindowclientconsultationbooker.cpp
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o $(OBJECTSPATH)moc_mainwindowclientconsultationbooker.o $(CLIENTPATH)moc_mainwindowclientconsultationbooker.cpp

$(OBJECTSPATH)Serveur.o: ServerReservation.c
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -I/usr/include/mysql -o $(OBJECTSPATH)Serveur.o ServerReservation.c

$(OBJECTSPATH)lib_socket.o: $(PROTOCOLSPATH)lib_socket.c
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -o $(OBJECTSPATH)lib_socket.o $(PROTOCOLSPATH)lib_socket.c

$(OBJECTSPATH)CBP.o : $(PROTOCOLSPATH)CBP.c
	g++ -Wno-unused-parameter -c -pipe -g -std=gnu++11 -Wall -W -D_REENTRANT -fPIC -DQT_DEPRECATED_WARNINGS -DQT_QML_DEBUG -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB -I../UNIX_DOSSIER_FINAL -I. -isystem /usr/include/qt5 -isystem /usr/include/qt5/QtWidgets -isystem /usr/include/qt5/QtGui -isystem /usr/include/qt5/QtCore -I. -I. -I/usr/lib64/qt5/mkspecs/linux-g++ -I/usr/include/mysql -o $(OBJECTSPATH)CBP.o $(PROTOCOLSPATH)CBP.c

clean:
	echo Nettoyage des fichiers temporaires
	rm -f $(OBJECTS) $(PROGRAMS)

run: all
	./Serveur