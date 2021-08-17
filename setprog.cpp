#include "setprog.h"
#include "ui_setprog.h"
#include "prog.h"
#include "xml.h"
#include <QFileDialog>
#include <moveprog.h>
#include <QMessageBox>


SetProg::SetProg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SetProg)
{
	ui->setupUi(this);
	setWindowTitle("Форма редактирования программы");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui->cBox_type->addItems({"Неизвестно", "Приложение", "Прошивка", "Документация"});
	setFlags();

	connect(ui->btn_dirIcon,
			SIGNAL(clicked(bool)),
			SLOT(slotSetIcon()));
	connect(ui->btn_dirDisc,
			SIGNAL(clicked(bool)),
			SLOT(slotSetDesc()));
	connect(ui->btn_false,
			SIGNAL(clicked(bool)),
			SLOT(reject()));
	connect(ui->btn_ok,
			SIGNAL(clicked(bool)),
			SLOT(slotExam()));
}

SetProg::~SetProg()
{
	delete ui;
}

void SetProg :: xmlAddProg(QVector<Prog> *m_mas_prog, QVector<MoveProg> *m_moveProg)
{
	Prog m_prog;
	MoveProg m_mProg;
	m_mProg.move = 0;
	m_mProg.appid = ui->lineEdit_appid->text();
	m_mProg.icon = ui->lineEdit_icon->text();
	m_mProg.desc = ui->lineEdit_desc->text();
	m_moveProg->push_back(m_mProg);

	m_prog.setAppid(ui->lineEdit_appid->text());
	m_prog.setDesc(QFileInfo(ui->lineEdit_desc->text()).fileName());
	m_prog.setIcon(QFileInfo(ui->lineEdit_icon->text()).fileName());
	m_prog.setName(ui->lineEdit_name->text());
	m_prog.setType(QString::number(ui->cBox_type->currentIndex()));
	m_mas_prog->push_back(m_prog);
}

void SetProg :: xmlEditProg(QVector<Prog> *m_mas_prog, int t_row,
							QVector<MoveProg> *m_moveProg, QVector<QStringList>* t_bufRemove)
{
	Prog m_prog;
	MoveProg m_mProg;
	QStringList removes;				//запоминает файлы, которые надо будет удалить
	m_mProg.move = 5;
	m_mProg.appid = ui->lineEdit_appid->text();
	m_prog.setAppid(ui->lineEdit_appid->text());
	flagDesc = false;
	flagIcon = false;
	if((*m_mas_prog)[t_row].getIcon() != QFileInfo(ui->lineEdit_icon->text()).fileName()){
		m_mProg.icon = ui->lineEdit_icon->text();
		m_prog.setIcon(QFileInfo(ui->lineEdit_icon->text()).fileName());
		removes << (*m_mas_prog)[t_row].getIcon();
		flagIcon = true;
	}
	if((*m_mas_prog)[t_row].getDesc() != QFileInfo(ui->lineEdit_desc->text()).fileName()){
		m_mProg.desc = ui->lineEdit_desc->text();
		m_prog.setDesc(QFileInfo(ui->lineEdit_desc->text()).fileName());
		removes << (*m_mas_prog)[t_row].getDesc();
		flagDesc = true;
	}
	if((*m_mas_prog)[t_row].getName() != ui->lineEdit_name->text())
		m_prog.setName(ui->lineEdit_name->text());

	if((*m_mas_prog)[t_row].getType() != QString::number(ui->cBox_type->currentIndex()))
			m_prog.setType(QString::number(ui->cBox_type->currentIndex()));
	m_moveProg->push_back(m_mProg);
	(*m_mas_prog)[t_row].replaceProg(m_prog);
	t_bufRemove->push_back(removes);
}

void SetProg :: slotSetIcon()
{
	QString icon = QFileDialog :: getOpenFileName(this, tr("Выберите значок"),
												  "C:/", tr("значок (*.png *.icon)") );
	if (icon !="")
		ui->lineEdit_icon->setText(icon);
}

void SetProg :: slotSetDesc()
{
	QString desc = QFileDialog :: getOpenFileName(this, tr("Выберите файл описания"),
												  "C:/", tr("Описание (*.txt)") );
	if (desc !="")
		ui->lineEdit_desc->setText(desc);
}

void SetProg :: slotExam()
{
	if(ui->lineEdit_appid->text() == ""){
		QMessageBox mbox(this);
		mbox.setText("Введите ID приложения");
		mbox.exec();
		return;
	} else if(!QFileInfo::exists(ui->lineEdit_desc->text()) && flagDesc){
		QMessageBox mbox(this);
		mbox.setText("Файла описания по введенному пути не существует");
		mbox.exec();
		return;
	} else if(ui->lineEdit_icon->text() != "" && !QFileInfo::exists(ui->lineEdit_desc->text()) && flagIcon){
		QMessageBox mbox(this);
		mbox.setText("Значка по введенному пути не существует");
		mbox.exec();
		return;
	} else if(ui->lineEdit_name->text() == ""){
		QMessageBox mbox(this);
		mbox.setText("Введите название программы");
		mbox.exec();
		return;
	} else {
		return QDialog::accept();
	}
}

void SetProg :: setAll(Prog t_prog)
{
	ui->lineEdit_appid->setText(t_prog.getAppid());
	ui->lineEdit_desc->setText(t_prog.getDesc());
	ui->lineEdit_icon->setText(t_prog.getIcon());
	ui->lineEdit_name->setText(t_prog.getName());
	ui->cBox_type->setCurrentIndex(t_prog.getType().toInt());
}

void SetProg :: setIdReadOnly(bool ok)
{
	ui->lineEdit_appid->setReadOnly(ok);
}

void SetProg :: setFlags()
{
	flagDesc = false;
	flagIcon = false;
}
