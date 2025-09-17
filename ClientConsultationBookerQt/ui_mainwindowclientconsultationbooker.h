/********************************************************************************
** Form generated from reading UI file 'mainwindowclientconsultationbooker.ui'
**
** Created by: Qt User Interface Compiler version 5.12.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOWCLIENTCONSULTATIONBOOKER_H
#define UI_MAINWINDOWCLIENTCONSULTATIONBOOKER_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindowClientConsultationBooker
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLineEdit *lineEditLastName;
    QLabel *label_9;
    QLabel *label_2;
    QLineEdit *lineEditFirstName;
    QLabel *label_3;
    QCheckBox *checkBoxNewPatient;
    QPushButton *pushButtonLogin;
    QPushButton *pushButtonLogout;
    QLabel *label_10;
    QLabel *label_4;
    QComboBox *comboBoxSpecialties;
    QLabel *label_5;
    QComboBox *comboBoxDoctors;
    QPushButton *pushButtonRechercher;
    QLabel *label_11;
    QTableWidget *tableWidgetConsultations;
    QPushButton *pushButtonReserver;
    QSpinBox *spinBoxId;
    QLabel *label_6;
    QDateEdit *dateEditStartDate;
    QLabel *label_7;
    QDateEdit *dateEditEndDate;
    QLabel *label_8;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindowClientConsultationBooker)
    {
        if (MainWindowClientConsultationBooker->objectName().isEmpty())
            MainWindowClientConsultationBooker->setObjectName(QString::fromUtf8("MainWindowClientConsultationBooker"));
        MainWindowClientConsultationBooker->resize(690, 507);
        centralwidget = new QWidget(MainWindowClientConsultationBooker);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 40, 64, 21));
        lineEditLastName = new QLineEdit(centralwidget);
        lineEditLastName->setObjectName(QString::fromUtf8("lineEditLastName"));
        lineEditLastName->setGeometry(QRect(100, 40, 191, 25));
        label_9 = new QLabel(centralwidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 10, 301, 21));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 70, 64, 21));
        lineEditFirstName = new QLineEdit(centralwidget);
        lineEditFirstName->setObjectName(QString::fromUtf8("lineEditFirstName"));
        lineEditFirstName->setGeometry(QRect(100, 70, 191, 25));
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 100, 81, 21));
        checkBoxNewPatient = new QCheckBox(centralwidget);
        checkBoxNewPatient->setObjectName(QString::fromUtf8("checkBoxNewPatient"));
        checkBoxNewPatient->setGeometry(QRect(10, 130, 161, 23));
        pushButtonLogin = new QPushButton(centralwidget);
        pushButtonLogin->setObjectName(QString::fromUtf8("pushButtonLogin"));
        pushButtonLogin->setGeometry(QRect(10, 160, 131, 25));
        pushButtonLogin->setStyleSheet(QString::fromUtf8("background-color: lightgreen;"));
        pushButtonLogout = new QPushButton(centralwidget);
        pushButtonLogout->setObjectName(QString::fromUtf8("pushButtonLogout"));
        pushButtonLogout->setGeometry(QRect(160, 160, 131, 25));
        pushButtonLogout->setStyleSheet(QString::fromUtf8("background-color: orange"));
        label_10 = new QLabel(centralwidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(330, 10, 321, 21));
        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(330, 40, 81, 21));
        comboBoxSpecialties = new QComboBox(centralwidget);
        comboBoxSpecialties->setObjectName(QString::fromUtf8("comboBoxSpecialties"));
        comboBoxSpecialties->setGeometry(QRect(410, 40, 271, 25));
        label_5 = new QLabel(centralwidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(330, 70, 71, 21));
        comboBoxDoctors = new QComboBox(centralwidget);
        comboBoxDoctors->setObjectName(QString::fromUtf8("comboBoxDoctors"));
        comboBoxDoctors->setGeometry(QRect(410, 70, 271, 25));
        pushButtonRechercher = new QPushButton(centralwidget);
        pushButtonRechercher->setObjectName(QString::fromUtf8("pushButtonRechercher"));
        pushButtonRechercher->setGeometry(QRect(330, 144, 351, 41));
        pushButtonRechercher->setStyleSheet(QString::fromUtf8("background-color: lightblue;\n"
"font-size: 16px;"));
        label_11 = new QLabel(centralwidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 210, 291, 21));
        tableWidgetConsultations = new QTableWidget(centralwidget);
        tableWidgetConsultations->setObjectName(QString::fromUtf8("tableWidgetConsultations"));
        tableWidgetConsultations->setEnabled(true);
        tableWidgetConsultations->setGeometry(QRect(10, 240, 671, 192));
        tableWidgetConsultations->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidgetConsultations->setSelectionBehavior(QAbstractItemView::SelectRows);
        pushButtonReserver = new QPushButton(centralwidget);
        pushButtonReserver->setObjectName(QString::fromUtf8("pushButtonReserver"));
        pushButtonReserver->setGeometry(QRect(10, 440, 671, 41));
        pushButtonReserver->setStyleSheet(QString::fromUtf8("background-color: lightsalmon;\n"
"font-size: 16px;"));
        spinBoxId = new QSpinBox(centralwidget);
        spinBoxId->setObjectName(QString::fromUtf8("spinBoxId"));
        spinBoxId->setGeometry(QRect(100, 100, 191, 26));
        spinBoxId->setMinimum(1);
        spinBoxId->setMaximum(99999);
        label_6 = new QLabel(centralwidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(330, 100, 64, 21));
        dateEditStartDate = new QDateEdit(centralwidget);
        dateEditStartDate->setObjectName(QString::fromUtf8("dateEditStartDate"));
        dateEditStartDate->setGeometry(QRect(460, 100, 91, 26));
        dateEditStartDate->setDate(QDate(2025, 9, 15));
        label_7 = new QLabel(centralwidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(410, 100, 64, 21));
        dateEditEndDate = new QDateEdit(centralwidget);
        dateEditEndDate->setObjectName(QString::fromUtf8("dateEditEndDate"));
        dateEditEndDate->setGeometry(QRect(589, 100, 91, 26));
        dateEditEndDate->setDate(QDate(2025, 12, 31));
        label_8 = new QLabel(centralwidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(560, 100, 21, 21));
        MainWindowClientConsultationBooker->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindowClientConsultationBooker);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 690, 22));
        MainWindowClientConsultationBooker->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindowClientConsultationBooker);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindowClientConsultationBooker->setStatusBar(statusbar);

        retranslateUi(MainWindowClientConsultationBooker);

        QMetaObject::connectSlotsByName(MainWindowClientConsultationBooker);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindowClientConsultationBooker)
    {
        MainWindowClientConsultationBooker->setWindowTitle(QApplication::translate("MainWindowClientConsultationBooker", "R\303\251servation d'une consultation", nullptr));
        label->setText(QApplication::translate("MainWindowClientConsultationBooker", "Nom :", nullptr));
        label_9->setText(QApplication::translate("MainWindowClientConsultationBooker", "<html><head/><body><p><span style=\" font-size:14pt; font-weight:600; color:#204a87;\">Coordonn\303\251es du patient :</span></p></body></html>", nullptr));
        label_2->setText(QApplication::translate("MainWindowClientConsultationBooker", "Pr\303\251nom :", nullptr));
        label_3->setText(QApplication::translate("MainWindowClientConsultationBooker", "No Patient :", nullptr));
        checkBoxNewPatient->setText(QApplication::translate("MainWindowClientConsultationBooker", "Nouveau Patient", nullptr));
        pushButtonLogin->setText(QApplication::translate("MainWindowClientConsultationBooker", "Login", nullptr));
        pushButtonLogout->setText(QApplication::translate("MainWindowClientConsultationBooker", "Logout", nullptr));
        label_10->setText(QApplication::translate("MainWindowClientConsultationBooker", "<html><head/><body><p><span style=\" font-size:14pt; font-weight:600; color:#204a87;\">Recherche de consultations :</span></p></body></html>", nullptr));
        label_4->setText(QApplication::translate("MainWindowClientConsultationBooker", "Sp\303\251cialit\303\251 :", nullptr));
        label_5->setText(QApplication::translate("MainWindowClientConsultationBooker", "M\303\251decin :", nullptr));
        pushButtonRechercher->setText(QApplication::translate("MainWindowClientConsultationBooker", "Lancer la recherche", nullptr));
        label_11->setText(QApplication::translate("MainWindowClientConsultationBooker", "<html><head/><body><p><span style=\" font-size:14pt; font-weight:600; color:#204a87;\">Consultations trouv\303\251es :</span></p></body></html>", nullptr));
        pushButtonReserver->setText(QApplication::translate("MainWindowClientConsultationBooker", "R\303\251server", nullptr));
        label_6->setText(QApplication::translate("MainWindowClientConsultationBooker", "Dates :", nullptr));
        label_7->setText(QApplication::translate("MainWindowClientConsultationBooker", "entre", nullptr));
        label_8->setText(QApplication::translate("MainWindowClientConsultationBooker", "et", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindowClientConsultationBooker: public Ui_MainWindowClientConsultationBooker {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOWCLIENTCONSULTATIONBOOKER_H
