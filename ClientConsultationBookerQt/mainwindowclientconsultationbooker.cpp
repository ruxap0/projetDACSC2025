#include "mainwindowclientconsultationbooker.h"
#include "ui_mainwindowclientconsultationbooker.h"
#include <QInputDialog>
#include <sys/types.h>
#include <sys/socket.h>
#include <QMessageBox>
#include <cstring>
#include <iostream>
#include "./protocols/lib_socket.h"
#include <signal.h>
#include <unistd.h>

using namespace std;

typedef struct {
    int id;
    char last_name[50];
    char first_name[50];
}DOCTORS;

int sClient = -1;

void handlerSIGINT(int sig);


MainWindowClientConsultationBooker::MainWindowClientConsultationBooker(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowClientConsultationBooker)
{
    // Armer SIGINT
    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = handlerSIGINT;
    sigaction(SIGINT,&A,NULL);


    ui->setupUi(this);
    logoutOk();

    // Configuration de la table des employes (Personnel Garage)
    ui->tableWidgetConsultations->setColumnCount(5);
    ui->tableWidgetConsultations->setRowCount(0);
    QStringList labelsTableConsultations;
    labelsTableConsultations << "Id" << "Spécialité" << "Médecin" << "Date" << "Heure";
    ui->tableWidgetConsultations->setHorizontalHeaderLabels(labelsTableConsultations);
    ui->tableWidgetConsultations->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetConsultations->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetConsultations->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetConsultations->horizontalHeader()->setVisible(true);
    ui->tableWidgetConsultations->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidgetConsultations->verticalHeader()->setVisible(false);
    ui->tableWidgetConsultations->horizontalHeader()->setStyleSheet("background-color: lightyellow");
    int columnWidths[] = {40, 150, 200, 150, 100};
    for (int col = 0; col < 5; ++col)
        ui->tableWidgetConsultations->setColumnWidth(col, columnWidths[col]);

}

MainWindowClientConsultationBooker::~MainWindowClientConsultationBooker()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles Table des livres encodés (ne pas modifier) ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientConsultationBooker::addTupleTableConsultations(int id,
                                                                    string specialty,
                                                                    string doctor,
                                                                    string date,
                                                                    string hour)
{
    int nb = ui->tableWidgetConsultations->rowCount();
    nb++;
    ui->tableWidgetConsultations->setRowCount(nb);
    ui->tableWidgetConsultations->setRowHeight(nb-1,10);

    // id
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::number(id));
    ui->tableWidgetConsultations->setItem(nb-1,0,item);

    // specialty
    item = new QTableWidgetItem;
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(specialty));
    ui->tableWidgetConsultations->setItem(nb-1,1,item);

    // doctor
    item = new QTableWidgetItem;
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(doctor));
    ui->tableWidgetConsultations->setItem(nb-1,2,item);

    // date
    item = new QTableWidgetItem;
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(date));
    ui->tableWidgetConsultations->setItem(nb-1,3,item);

    // hour
    item = new QTableWidgetItem;
    item->setTextAlignment(Qt::AlignCenter);
    item->setText(QString::fromStdString(hour));
    ui->tableWidgetConsultations->setItem(nb-1,4,item);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientConsultationBooker::clearTableConsultations() {
    ui->tableWidgetConsultations->setRowCount(0);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MainWindowClientConsultationBooker::getSelectionIndexTableConsultations() const
{
    QModelIndexList list = ui->tableWidgetConsultations->selectionModel()->selectedRows();
    if (list.size() == 0) return -1;
    QModelIndex index = list.at(0);
    int ind = index.row();
    return ind;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles des comboboxes (ne pas modifier) //////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientConsultationBooker::addComboBoxSpecialties(string specialty) {
    ui->comboBoxSpecialties->addItem(QString::fromStdString(specialty));
}

string MainWindowClientConsultationBooker::getSelectionSpecialty() const {
    return ui->comboBoxSpecialties->currentText().toStdString();
}

void MainWindowClientConsultationBooker::clearComboBoxSpecialties() {
    ui->comboBoxSpecialties->clear();
    this->addComboBoxSpecialties("--- TOUTES ---");
}

void MainWindowClientConsultationBooker::addComboBoxDoctors(string doctor) {
    ui->comboBoxDoctors->addItem(QString::fromStdString(doctor));
}

string MainWindowClientConsultationBooker::getSelectionDoctor() const {
    return ui->comboBoxDoctors->currentText().toStdString();
}

void MainWindowClientConsultationBooker::clearComboBoxDoctors() {
    ui->comboBoxDoctors->clear();
    this->addComboBoxDoctors("--- TOUS ---");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonction utiles de la fenêtre (ne pas modifier) ////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientConsultationBooker::getLastName() const {
    return ui->lineEditLastName->text().toStdString();
}

string MainWindowClientConsultationBooker::getFirstName() const {
    return ui->lineEditFirstName->text().toStdString();
}

int MainWindowClientConsultationBooker::getPatientId() const {
    return ui->spinBoxId->value();
}

void MainWindowClientConsultationBooker::setLastName(string value) {
    ui->lineEditLastName->setText(QString::fromStdString(value));
}

string MainWindowClientConsultationBooker::getStartDate() const {
    return ui->dateEditStartDate->date().toString("yyyy-MM-dd").toStdString();
}

string MainWindowClientConsultationBooker::getEndDate() const {
    return ui->dateEditEndDate->date().toString("yyyy-MM-dd").toStdString();
}

void MainWindowClientConsultationBooker::setFirstName(string value) {
    ui->lineEditFirstName->setText(QString::fromStdString(value));
}

void MainWindowClientConsultationBooker::setPatientId(int value) {
    if (value > 0) ui->spinBoxId->setValue(value);
}

bool MainWindowClientConsultationBooker::isNewPatientSelected() const {
    return ui->checkBoxNewPatient->isChecked();
}

void MainWindowClientConsultationBooker::setNewPatientChecked(bool state) {
    ui->checkBoxNewPatient->setChecked(state);
}

void MainWindowClientConsultationBooker::setStartDate(string date) {
    QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
    if (qdate.isValid()) ui->dateEditStartDate->setDate(qdate);
}

void MainWindowClientConsultationBooker::setEndDate(string date) {
    QDate qdate = QDate::fromString(QString::fromStdString(date), "yyyy-MM-dd");
    if (qdate.isValid()) ui->dateEditEndDate->setDate(qdate);
}

void MainWindowClientConsultationBooker::loginOk() {
    ui->lineEditLastName->setReadOnly(true);
    ui->lineEditFirstName->setReadOnly(true);
    ui->spinBoxId->setReadOnly(true);
    ui->checkBoxNewPatient->setEnabled(false);
    ui->pushButtonLogout->setEnabled(true);
    ui->pushButtonLogin->setEnabled(false);
    ui->pushButtonRechercher->setEnabled(true);
    ui->pushButtonReserver->setEnabled(true);

    int ret;
    char requete[256];
    char reponse[256];

    sprintf(requete, "GET_SPECIALTIES");
    if((ret = Send(sClient, requete, strlen(requete))) < 0)
    {
        perror("Erreur de Send");
        exit(1);
    }

    if((ret = Receive(sClient, reponse)) < 0)
    {
        perror("Erreur de Receive");
        exit(1);
    }

    char* ptr = strtok(reponse, "#");

    if(strcmp(ptr, "OK") == 0)
    {
        int i = 0;
        clearComboBoxSpecialties();
        while((ptr = strtok(NULL, "#")) != NULL)
        {
            if(i % 2 == 1)
                this->addComboBoxSpecialties(string(ptr));
            i++;
        }
    }
    else
    {
        dialogError("Erreur", "Récupération des spécialités échouée");
    }

    sprintf(requete, "GET_DOCTORS");
    if((ret = Send(sClient, requete, strlen(requete))) < 0)
    {
        perror("Erreur de Send");
        exit(1);
    }

    if((ret = Receive(sClient, reponse)) < 0)
    {
        perror("Erreur de Receive");
        exit(1);
    }

    ptr = strtok(reponse, "#");

    if(strcmp(ptr, "OK") == 0)
    {
        int i = 0;
        clearComboBoxDoctors();
        while((ptr = strtok(NULL, "#")) != NULL)
        {
            if(i % 2 == 1)
            {
                char doctor[100] = "";
                strcpy(doctor, ptr);
                strcat(doctor, " ");
                strcat(doctor, strtok(NULL, "#"));
                cout << "doctor = " << doctor << endl;

                this->addComboBoxDoctors(string(doctor));
            }
            i++;

        }
    }
    else
    {
        dialogError("Erreur", "Récupération des médecins échouée");
    }




}

void MainWindowClientConsultationBooker::logoutOk() {
    ui->lineEditLastName->setReadOnly(false);
    setLastName("");
    ui->lineEditFirstName->setReadOnly(false);
    setFirstName("");
    ui->spinBoxId->setReadOnly(false);
    setPatientId(1);
    ui->checkBoxNewPatient->setEnabled(true);
    setNewPatientChecked(false);
    ui->pushButtonLogout->setEnabled(false);
    ui->pushButtonLogin->setEnabled(true);
    ui->pushButtonRechercher->setEnabled(false);
    ui->pushButtonReserver->setEnabled(false);
    setStartDate("2025-09-15");
    setEndDate("2025-12-31");
    clearComboBoxDoctors();
    clearComboBoxSpecialties();
    clearTableConsultations();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue (ne pas modifier) ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientConsultationBooker::dialogMessage(const string& title,const string& message) {
   QMessageBox::information(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientConsultationBooker::dialogError(const string& title,const string& message) {
   QMessageBox::critical(this,QString::fromStdString(title),QString::fromStdString(message));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
string MainWindowClientConsultationBooker::dialogInputText(const string& title,const string& question) {
    return QInputDialog::getText(this,QString::fromStdString(title),QString::fromStdString(question)).toStdString();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int MainWindowClientConsultationBooker::dialogInputInt(const string& title,const string& question) {
    return QInputDialog::getInt(this,QString::fromStdString(title),QString::fromStdString(question));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions gestion des boutons (TO DO) //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindowClientConsultationBooker::on_pushButtonLogin_clicked()
{
    int ret;
    string lastName = this->getLastName();
    string firstName = this->getFirstName();
    int patientId = this->getPatientId();
    bool newPatient = this->isNewPatientSelected();

    cout << "lastName = " << lastName << endl;
    cout << "FirstName = " << firstName << endl;
    cout << "patientId = " << patientId << endl;
    cout << "newPatient = " << newPatient << endl;

    // Envoi d'une requete login au serveur
    if ((sClient = ClientSocket(const_cast<char*>("127.0.0.1"), 50005)) == -1)
    {
        perror("Erreur de ClientSocket");
        exit(1);
    }

    char requete[256];
    if (newPatient)
        sprintf(requete, "LOGIN#%s#%s#%d#NEW", lastName.c_str(), firstName.c_str(), patientId);
    else
        sprintf(requete, "LOGIN#%s#%s#%d#NOT", lastName.c_str(), firstName.c_str(), patientId);

    char reponse[256];
    if((ret = Send(sClient, requete, strlen(requete))) < 0)
    {
        perror("Erreur de Send");
        exit(1);
    }

    if((ret = Receive(sClient, reponse)) < 0)
    {
        perror("Erreur de Receive");
        exit(1);
    }

    if(strcmp(reponse, "OK") == 0)
        loginOk();
    else
    {
        dialogError("Erreur", "Login échoué");
    }
}

void MainWindowClientConsultationBooker::on_pushButtonLogout_clicked()
{
    // Envoi d'une requete logout au serveur
    if (sClient != -1)
    {
        char requete[256];
        sprintf(requete, "LOGOUT");

        char reponse[256];
        int ret;
        if((ret = Send(sClient, requete, strlen(requete))) < 0)
        {
            perror("Erreur de Send");
            exit(1);
        }

        if((ret = Receive(sClient, reponse)) < 0)
        {
            perror("Erreur de Receive");
            exit(1);
        }

        ::close(sClient);
        sClient = -1;
    }

    logoutOk();
}

void MainWindowClientConsultationBooker::on_pushButtonRechercher_clicked()
{
    string specialty = this->getSelectionSpecialty();
    string doctor = this->getSelectionDoctor();
    string startDate = this->getStartDate();
    string endDate = this->getEndDate();

    cout << "specialty = " << specialty << endl;
    cout << "doctor = " << doctor << endl;
    cout << "startDate = " << startDate << endl;
    cout << "endDate = " << endDate << endl;
}

void MainWindowClientConsultationBooker::on_pushButtonReserver_clicked()
{
    int selectedTow = this->getSelectionIndexTableConsultations();

    cout << "selectedRow = " << selectedTow << endl;
}

// ----------------------------------------------------------------------//
// ----- fonctions en + -------------------------------------------------//
// ----------------------------------------------------------------------//
void handlerSIGINT(int sig)
{
    close(sClient);
    printf("\nFermeture du client\n");
    exit(0);
}