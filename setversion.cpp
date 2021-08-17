#include "setversion.h"
#include "ui_setversion.h"
#include "versionlist.h"
#include <QFileDialog>
#include <QDate>
#include <QMessageBox>


setVersion::setVersion(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::setVersion)
{
	ui->setupUi(this);
	ui->cBox_type->setEditable(false);
	ui->cBox_type->addItems({"Неизвестная", "Устаревшая", "Не использовалась",
							 "Стабильная", "Нестабильная", "Выпуск"});
	setWindowTitle("Форма для редактирования версии программы");
	ui->lineEdit_date->setText(QDate::currentDate().toString("dd.MM.yyyy"));

	setFlags();
	m_xml = new XML;
	m_prog = new Prog;
	m_mas_prog = new QVector<Prog>;
//	m_MoveProg = new QVector<MoveProg>;
	m_VersionList = new VersionList;
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	connect(ui->btn_setCl,
			SIGNAL(clicked(bool)),
			SLOT(slotSetChangelog()));
	connect(ui->btn_SetInstall,
			SIGNAL(clicked(bool)),
			SLOT(slotSetInstall()));
	connect(ui->btn_SetZip,
			SIGNAL(clicked(bool)),
			SLOT(slotSetZip()));
	connect(ui->btn_ok,
			SIGNAL(clicked(bool)),
			SLOT(slotExam()));
	connect(ui->btn_false,
			SIGNAL(clicked(bool)),
			SLOT(reject()));
}

setVersion::~setVersion()
{
	delete m_prog;
	delete m_mas_prog;
	delete ui;
	delete m_MoveProg;
}

void setVersion::xml_replace(QVector<Prog> &m_mas_prog, int t_row, QVector<MoveProg> &m_MoveProg,
							 int t_rowVer, QVector<QStringList> &m_bufRemove)
{
	Version m_version = ((m_mas_prog)[t_row].getVersions())[t_rowVer];
	MoveProg m_mProge;
	Version m_ver;
	QStringList m_Removes;
	m_mProge.move = 4;
	m_mProge.id = ui->lineEdit_id_ver->text();
	if(m_version.getArchive() != ui->lineEdit_archive->text()){
		m_mProge.zip = ui->lineEdit_archive->text();
		m_ver.setArchive(QFileInfo(ui->lineEdit_archive->text()).fileName());
		m_Removes << m_version.getArchive();
		flagArchive=true;
	}
	if(m_version.getChangelog() != ui->lineEdit_cl->text()){
		m_mProge.changelog = ui->lineEdit_cl->text();
		m_ver.setChangelog(QFileInfo(ui->lineEdit_cl->text()).fileName());
		m_Removes << m_version.getChangelog();
		flagChangelog = true;
	}
	if(m_version.getInstaller() != ui->lineEdit_installer->text()){
		m_mProge.installer = ui->lineEdit_installer->text();
		m_ver.setInstaller(QFileInfo(ui->lineEdit_installer->text()).fileName());
		m_Removes << m_version.getInstaller();
		flagInstaller = true;
	}
	m_bufRemove.push_back(m_Removes);
	m_mProge.appid = m_mas_prog[t_row].getAppid();
	m_MoveProg.push_back(m_mProge);

	if(m_version.getDate() != ui->lineEdit_date->text())
		m_ver.setDate(ui->lineEdit_date->text());
	if(m_version.getDisplay() != ui->lineEdit_display->text())
		m_ver.setDisplay(ui->lineEdit_display->text());
	if(m_version.getMainfile() != ui->lineEdit_mf->text())
		m_ver.setMainfile(ui->lineEdit_mf->text());
	if(m_version.getType() != QString::number(ui->cBox_type->currentIndex()))
		m_ver.setType(QString::number(ui->cBox_type->currentIndex()));
	m_ver.setId(ui->lineEdit_id_ver->text());
	m_mas_prog[t_row].replaceVersion(t_rowVer, m_ver);
	qDebug() << "Replace version";
}

void setVersion::xml_add_ver(QVector<Prog> &m_mas_prog, int t_row, QVector<MoveProg> &m_MoveProg)
{
	MoveProg m_mProge;
	m_mProge.move = 2;
	m_mProge.id = ui->lineEdit_id_ver->text();
	m_mProge.zip = ui->lineEdit_archive->text();
	m_mProge.changelog = ui->lineEdit_cl->text();
	m_mProge.appid = m_mas_prog[t_row].getAppid();
	m_MoveProg.push_back(m_mProge);

	Version m_ver;
	m_ver.setArchive(ui->lineEdit_id_ver->text() + "/" +
						QFileInfo(ui->lineEdit_archive->text()).fileName());
	m_ver.setChangelog(ui->lineEdit_id_ver->text() + "/" +
						QFileInfo(ui->lineEdit_cl->text()).fileName());
	m_ver.setDate(ui->lineEdit_date->text());
	m_ver.setDisplay(ui->lineEdit_display->text());
	m_ver.setInstaller(ui->lineEdit_id_ver->text() + "/" +
						QFileInfo(ui->lineEdit_installer->text()).fileName());
	m_ver.setMainfile(ui->lineEdit_mf->text());
	m_ver.setType(QString::number(ui->cBox_type->currentIndex()));
	m_ver.setId(ui->lineEdit_id_ver->text());
	m_mas_prog[t_row].setVersionInVector(m_ver);
	qDebug() << "Add version";
}

void setVersion :: slotSetChangelog()
{
	QString path = QFileDialog :: getOpenFileName(this, tr("Выберите список изменений"),
												  "C:/", tr("Список изменений (*.txt)") );
	if (path !="")
		ui->lineEdit_cl->setText(path);
}

void setVersion :: slotSetInstall()
{
	QString path = QFileDialog :: getOpenFileName(this, tr("Выберите установочный файл"),
												  "C:/", tr("Установочный файл (*.exe)") );
	if (path !="")
		ui->lineEdit_installer->setText(path);
}

void setVersion :: slotSetZip()
{
	QString path = QFileDialog :: getOpenFileName(this, tr("Выберите архив"),
												  "C:/", tr("Архив (*.zip)") );
	if (path !=""){
		ui->lineEdit_archive->setText(path);
		m_archive = NULL;
		m_archive = archive_read_new();
		archive_read_support_filter_all(m_archive);
		archive_read_support_format_all(m_archive);
		QByteArray ba = path.toLocal8Bit();
		const char *path2 = ba.data();
		archive_read_open_filename(m_archive, path2, 10240);
		while (archive_read_next_header(m_archive, &m_entry) == ARCHIVE_OK){
			QString path3= archive_entry_pathname(m_entry);
			if((path3.lastIndexOf("exe")) != -1)
				ui->lineEdit_mf->setText(path3);
		}
	}
}

void setVersion :: slotExam()
{
	if(!QFileInfo::exists(ui->lineEdit_archive->text()) && flagArchive){
		QMessageBox mbox(this);
		mbox.setText("Архив по введенному пути не существует");
		mbox.exec();
		return;
	} else if(!QFileInfo::exists(ui->lineEdit_cl->text()) && flagChangelog){
		QMessageBox mbox(this);
		mbox.setText("Список изменений по введенному пути не существует");
		mbox.exec();
		return;
	} else if(flagInstaller && ui->lineEdit_installer->text() != ""
			  && !QFileInfo::exists(ui->lineEdit_installer->text())){
		QMessageBox mbox(this);
		mbox.setText("Установочный файл по введенному пути не существует");
		mbox.exec();
		return;
	} else if(ui->lineEdit_date->text() == ""){
		QMessageBox mbox(this);
		mbox.setText("Введите дату");
		mbox.exec();
		return;
	} else if(ui->lineEdit_display->text() == ""){
		QMessageBox mbox(this);
		mbox.setText("Введите версию");
		mbox.exec();
		return;
	} else if(ui->lineEdit_id_ver->text() == ""){
		QMessageBox mbox(this);
		mbox.setText("Введите ID версии");
		mbox.exec();
		return;
	} else if(ui->lineEdit_mf->text() == ""){
		QMessageBox mbox(this);
		mbox.setText("Введите файл для запуска");
		mbox.exec();
		return;
	} else {
		QDialog::accept();
	}
}

void setVersion :: setAll (Version t_version)
{
	ui->lineEdit_id_ver->setText(t_version.getId());
	ui->lineEdit_archive->setText(t_version.getArchive());
	ui->lineEdit_cl->setText(t_version.getChangelog());
	ui->lineEdit_date->setText(t_version.getDate());
	ui->lineEdit_display->setText(t_version.getDisplay());
	ui->lineEdit_installer->setText(t_version.getInstaller());
	ui->lineEdit_mf->setText(t_version.getMainfile());
	ui->cBox_type->setCurrentIndex(t_version.getType().toInt());
}

void setVersion :: setIdVer (QString t_idVer)
{
	this->ui->lineEdit_id_ver->setText(t_idVer);
}

void setVersion :: setChangelog(QString t_changelog)
{
	this->ui->lineEdit_cl->setText(t_changelog);
}

void setVersion :: setArchive(QString t_archive)
{
	this->ui->lineEdit_archive->setText(t_archive);
}

void setVersion :: setVerType(QString t_type)
{
	this->ui->cBox_type->setCurrentIndex(t_type.toInt());
}

void setVersion :: setDisplay(QString t_display)
{
	this->ui->lineEdit_display->setText(t_display);
}

void setVersion :: setMainfile(QString t_mainfile)
{
	this->ui->lineEdit_mf->setText(t_mainfile);
}

void setVersion :: setInstaller(QString t_installer)
{
	this->ui->lineEdit_installer->setText(t_installer);
}

void setVersion :: setDate(QString t_date)
{
	this->ui->lineEdit_date->setText(t_date);
}


QString setVersion :: getIdVer ()
{
	return this->ui->label_id->text();
}

QString setVersion :: getChangelog()
{
	return this->ui->label_changelog->text();
}

QString setVersion :: getArchive()
{
	return this->ui->label_archive->text();
}

QString setVersion :: getVerType()
{
	return this->ui->label_type->text();
}

QString setVersion :: getDisplay()
{
	return this->ui->label_display->text();
}

QString setVersion :: getMainfile()
{
	return this->ui->label_mainfile->text();
}

QString setVersion :: getInstaller()
{
	return this->ui->label_installer->text();
}

QString setVersion :: getDate()
{
	return this->ui->label_date->text();
}

void setVersion :: setIdReadOnly(bool ok)
{
	ui->lineEdit_id_ver->setReadOnly(ok);
}

void setVersion :: setFlags()
{
	flagArchive = false;
	flagChangelog = false;
	flagInstaller = false;
}
