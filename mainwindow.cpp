#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>
#include "sftpparams.h"
#include "xml.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->btn_update->setEnabled(false);
	ui->btn_add_prog->setEnabled(false);
	ui->btn_del_prog->setEnabled(false);
	ui->btn_save->setEnabled(false);
	ui->btn_edit_prog->setEnabled(false);
	setWindowTitle("plAdmin2");
	m_Loading = new Loading;
	m_sftpClient = new SftpClient(this);
	m_UserList = new UserList;
	m_UserList->exec();

	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	path = path + "/" + "plAdmin2/users.ini";
	QSettings m_SettList(path, QSettings::IniFormat);
	m_user = ((m_SettList.value("main/accepted")).toString());
	m_SettList.beginGroup(m_user);
	m_host = m_SettList.value("ip").toString();
	m_port = m_SettList.value("port").toInt();
	m_repo = m_SettList.value("repo").toString();

	connect(ui->btn_connect,
			SIGNAL(clicked(bool)),
			SLOT(slotConnectButton()));
	connect(m_sftpClient,
			SIGNAL(sigConnectToHost(bool)),
			SLOT(slotConnected(bool)));
	connect(m_sftpClient,
			SIGNAL(sigDisconnectFromHost()),
			SLOT(slotDisconnected()));
	connect(m_sftpClient,
			SIGNAL(sigConfirmServerKey(QString)),
			SLOT(slotCheckServerKey(QString)));
	connect(m_sftpClient,
			SIGNAL(sigNeedPassword()),
			SLOT(slotNeedPassword()));;
	connect(m_sftpClient,
			SIGNAL(sigMkdir(bool, QString)),
			m_Loading,
			SLOT(slotMkdir(bool, QString)));
	connect(m_sftpClient,
			SIGNAL(sigRmdir(bool, QString)),
			m_Loading,
			SLOT(slotRmdir(bool, QString)));
	connect(m_sftpClient,
			SIGNAL(sigUnlink(bool, QString)),
			m_Loading,
			SLOT(slotRemove(bool, QString)));
	connect(m_sftpClient,
			SIGNAL(sigUploadFile(bool, QString)),
			m_Loading,
			SLOT(slotUpload(bool, QString)));
	connect(m_sftpClient,
			SIGNAL(sigDownloadFile(bool, QString)),
			m_Loading,
			SLOT(slotDownload(bool, QString)));
	connect(m_sftpClient,
			SIGNAL(sigDownloadFile(bool, QString)),
			SLOT(slotDownload(bool, QString)));
	connect(m_sftpClient,
			SIGNAL(sigGetDir(bool, QStringList, QStringList)),
			SLOT(slotGetdir(bool, QStringList, QStringList)));
	connect(ui->btn_users,
			SIGNAL(clicked(bool)),
			SLOT(slotUsersList()));
	connect(ui->tableView,
			SIGNAL(doubleClicked(QModelIndex)),
			SLOT(slotpProgSelect(QModelIndex)));
	connect(ui->btn_add_prog,
			SIGNAL(clicked(bool)),
			SLOT(slotAddProg()));
	connect(ui->btn_del_prog,
			SIGNAL(clicked(bool)),
			SLOT(slotDeleteProg()));
	connect(ui->btn_update,
			SIGNAL(clicked(bool)),
			SLOT(slotXMLbutton()));
	connect(ui->btn_save,
			SIGNAL(clicked(bool)),
			SLOT(slotSaveButton()));
	connect(ui->btn_edit_prog,
			SIGNAL(clicked(bool)),
			SLOT(slotEditProg()));
}

MainWindow::~MainWindow()
{
	delete ui;
	delete m_sftpClient;
	delete m_UserList;
	delete m_Loading;
}

void MainWindow::slotConnectButton()
{
	if (!m_connected) {
		m_sftpClient->setAddr(m_host);
		m_sftpClient->setUsername("user");
		m_sftpClient->setPort(22);
		m_sftpClient->connectToHost();
	} else {
		m_sftpClient->disconnectFromHost();
	}
	ui->btn_connect->setEnabled(false);
}

void MainWindow::slotConnected(bool ok)
{
	if(ok)
	{
		ui->btn_connect->setEnabled(true);
		QMessageBox mbox(this);
		mbox.setText("Подключение установлено");
		m_connected = true;
		ui->btn_connect->setText("Отключиться");
		ui->btn_update->setEnabled(true);
		mbox.exec();
	} else {
		ui->btn_connect->setEnabled(true);
		QMessageBox mbox(this);
		mbox.setText("Подключение не установлено");
		mbox.exec();
	}
}

void MainWindow::slotDisconnected()
{
	qDebug() << "Disconnected";

	m_connected = false;
	ui->btn_connect->setText("Подключиться");
	ui->btn_connect->setEnabled(true);
	ui->btn_update->setEnabled(false);
	ui->btn_add_prog->setEnabled(false);
	ui->btn_del_prog->setEnabled(false);
	ui->btn_save->setEnabled(false);
	ui->btn_edit_prog->setEnabled(false);

}

void MainWindow::slotCheckServerKey(const QString &sha1)
{
	QString msg = tr("Вы подключаетесь к этому серверу в первый раз, "
					"или ключ сервера изменился, "
					"или это MITM. Продолжить?");

	QMessageBox mbox(this);
	mbox.setIcon(QMessageBox::Question);
	mbox.setText(msg);
	mbox.setDetailedText(sha1);
	mbox.setWindowTitle(tr("Внимание"));
	mbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

	int ans = mbox.exec();
	if (ans == QMessageBox::Yes) {
		m_sftpClient->confirmServerKey();
	} else {
		m_sftpClient->disconnectFromHost();
	}
}

void MainWindow::slotNeedPassword()
{
	QInputDialog diaPw(this);
	diaPw.setInputMode(QInputDialog::TextInput);
	diaPw.setTextEchoMode(QLineEdit::Password);
	diaPw.setLabelText(tr("Введите пароль:"));
	if (diaPw.exec() != QInputDialog::Accepted) {
		m_sftpClient->disconnectFromHost();
	} else {
		m_sftpClient->providePassword(diaPw.textValue());
	}
}

void MainWindow::slotMkdir(bool ok, QString path)
{
	qDebug() << "mkdir" << path << " is " << ok;
}

void MainWindow::slotRmdir(bool ok, QString path)
{
	qDebug() << "rmdir" << path << " is " << ok;
}

void MainWindow::slotRemove(bool ok, QString path)
{
	qDebug() << "unlink" << path << " is " << ok;
}

void MainWindow::slotUpload(bool ok, QString localPath)
{
	qDebug() << "upload" << localPath << " is " << ok;
}

void MainWindow::slotDownload(bool ok, QString remotePath)
{
	qDebug() << "download " << remotePath << " is " << ok;
	if (ok){
		XML m_xml;
		m_xml.readXML(m_QVectorProg);
		this->SetTable();
	}
}

void MainWindow::slotGetdir(bool ok, QStringList list_name, QStringList list_type)			//отображала список файлов в дир
{
	qDebug() << "getdir" << ok;
//	ui->listWidget->clear();
//	ui->listWidget->addItems(list_name);
}

void MainWindow :: slotXMLbutton()
{
	SftpDownloadParams p;
	p.localPath= "repo2.xml";
	p.remotePath=m_repo+"repo2.xml";
	m_sftpClient->downloadFile(p);
	m_sizeOperation++;

//	SftpUploadParams p;
//	p.localPath= "D:/repo2.xml";
//	p.remotePath=m_repo+"repo2.xml";
//	m_sftpClient->uploadFile(p);
}

void MainWindow :: slotSaveButton()
{
	int i_remove=0;
	XML m_xml;
	m_xml.editXML(m_QVectorProg);
	m_Loading->show();
	QString path;
	for(const MoveProg &item: m_moveProg){
		switch (item.move) {
		case 0:
			if(item.icon != ""){
				addProg(item.appid, item.desc, item.icon);
			} else {
				addProg(item.appid, item.desc);
			}
			break;
		case 1:
			deleteProg(item.appid);
			break;
		case 2:
			addVersion(item.id, item.zip, item.changelog, item.installer, item.appid);
			break;
		case 3:
			deleteVersion(item.id, item.appid);
			break;
		case 4:
			path = m_repo + item.appid + "/" +item.id + "/";
			for(const QString &jtem : m_buff_remove[i_remove])
				m_sftpClient->unlink(path+jtem);
			editVersion(item.id, item.zip, item.changelog, item.installer, item.appid);
			break;
		case 5:
			path = m_repo + item.appid + "/";
			for(const QString &jtem : m_buff_remove[i_remove])
				m_sftpClient->unlink(path+jtem);
			editProg(item.appid, item.desc, item.icon);
			break;
		default:
			break;
		}
		i_remove++;
	}
	SftpUploadParams p;
	p.localPath = "repo2.xml";
	p.remotePath = m_repo + "repo2.xml";
	m_sftpClient->uploadFile(p);
	m_sizeOperation++;
	m_moveProg.clear();
	m_Loading->setSize(m_sizeOperation);
	m_sizeOperation = 0;
}

void MainWindow :: slotUsersList()
{
	m_UserList = new UserList;
	m_UserList->exec();
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	path = path + "/" + "plAdmin2/users.ini";
	QSettings m_SettList(path, QSettings::IniFormat);
	m_user = ((m_SettList.value("main/accepted")).toString());
	m_SettList.beginGroup(m_user);
	m_host = m_SettList.value("ip").toString();
	m_port = m_SettList.value("port").toInt();
	m_repo = m_SettList.value("repo").toString();
}

void MainWindow :: slotDeleteProg()
{
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){
		int row = selectModel -> selectedRows().first().row();
		MoveProg m_mProg;
		m_mProg.appid = m_QVectorProg[row].getAppid();
		m_mProg.move = 1;
		m_moveProg.push_back(m_mProg);
		m_QVectorProg.remove(row);
		SetTable();
	} else {
		QMessageBox msg;
		msg.setText("Программа для удаления не выбрана");
		msg.exec();
	}
}

void MainWindow :: slotAddProg()
{
	SetProg m_SetProg;
	if(m_SetProg.exec()==QDialog::Accepted){
		m_SetProg.xmlAddProg(&m_QVectorProg, &m_moveProg);
		SetTable();
	}
}

void MainWindow :: slotEditProg()
{
	SetProg m_SetProg;
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){
		int row = selectModel -> selectedRows().first().row();
		m_SetProg.setIdReadOnly(true);
		m_SetProg.setAll(m_QVectorProg[row]);
		if(m_SetProg.exec()==QDialog::Accepted){
			m_SetProg.xmlEditProg(&m_QVectorProg, row, &m_moveProg, &m_buff_remove);
			SetTable();
		}
	}
}

void MainWindow :: slotpProgSelect(const QModelIndex & index )
{
	m_VersionList = new VersionList;
	m_VersionList->SetRow(index.row());
	m_VersionList -> GetList(m_QVectorProg[m_VersionList->GetRow()]);
	m_VersionList->SetQVectorProg(m_QVectorProg);
	m_VersionList->SetQVectorMoveProg(m_moveProg);
	m_VersionList->SetBufferRemove(m_buff_remove);
	if (m_VersionList->exec() == QDialog::Accepted){
		m_VersionList->PushQVectorProg(m_QVectorProg);
		m_VersionList->PushQVectorMoveProg(m_moveProg);
		m_VersionList->PushBufferRemove(m_buff_remove);
	}
}

void MainWindow :: SetTable()
{
	QStandardItemModel *model = new QStandardItemModel;
	QStandardItem *item;
	QStringList horizontalHeader;
	horizontalHeader.append("Иконка");
	horizontalHeader.append("Имя");
	horizontalHeader.append("Тип");

	model -> setHorizontalHeaderLabels(horizontalHeader);
	for(int i = 0; i < m_QVectorProg.size(); i++){
		QStandardItem *icon = new QStandardItem;
		icon->setIcon(QIcon(m_QVectorProg[i].getIcon()));
		model->appendRow(icon);
		item = new QStandardItem((m_QVectorProg)[i].getName());
		model -> setItem(i, 1, item);
		switch((((m_QVectorProg)[i].getType()).toInt())){
		case 0:
			item = new QStandardItem("Неизвестно");
			break;
		case 1:
			item = new QStandardItem("Приложение");
			break;
		case 2:
			item = new QStandardItem("Прошивка");
			break;
		case 3:
			item = new QStandardItem("Документация");
			break;
		}
		model -> setItem(i, 2, item);
	}
	ui -> tableView -> setModel(model);
	ui -> tableView -> resizeColumnsToContents();
	ui -> tableView -> resizeRowsToContents();
	ui->btn_add_prog->setEnabled(true);
	ui->btn_del_prog->setEnabled(true);
	ui->btn_save->setEnabled(true);
	ui->btn_edit_prog->setEnabled(true);
}

void MainWindow:: addProg(QString id, QString desc, QString icon)
{
	QString path = m_repo + id + "/";
	m_sftpClient->mkdir(path, S_IRWXU);
	m_sizeOperation++;
	SftpUploadParams p;
	p.localPath = desc;
	p.remotePath = path + QFileInfo(desc).fileName();
	m_sftpClient->uploadFile(p);
	m_sizeOperation++;
	p.localPath = icon;
	p.remotePath = path + QFileInfo(icon).fileName();
	QFile::copy(p.localPath, QFileInfo(icon).fileName());
	m_sftpClient->uploadFile(p);
	m_sizeOperation++;
}

void MainWindow:: addProg(QString id, QString desc)
{
	QString path = m_repo + id + "/";
	m_sftpClient->mkdir(path, S_IRWXU);
	m_sizeOperation++;
	SftpUploadParams p;
	p.localPath = desc;
	p.remotePath = path + (QFileInfo(desc).fileName());
	m_sftpClient->uploadFile(p);
	m_sizeOperation++;
}

void MainWindow :: addVersion(QString id, QString zip, QString changelog, QString installer, QString appid)
{
	QString path = m_repo + appid + "/" + id + "/";
	m_sftpClient->mkdir(path, S_IRWXU);
	m_sizeOperation++;
	if(zip != ""){
		SftpUploadParams p;
		p.localPath = zip;
		p.remotePath = path + (QFileInfo(zip).fileName());
		m_sftpClient -> uploadFile(p);
		m_sizeOperation++;
	}
	if(changelog != ""){
		SftpUploadParams p;
		p.localPath = changelog;
		p.remotePath = path + (QFileInfo(changelog).fileName());
		m_sftpClient -> uploadFile(p);
		m_sizeOperation++;
	}
	if(installer != ""){
		SftpUploadParams p;
		p.localPath = installer;
		p.remotePath = path + (QFileInfo(installer).fileName());
		m_sftpClient -> uploadFile(p);
		m_sizeOperation++;
	}
}

void MainWindow :: deleteProg(QString id)
{
	m_sftpClient->getDirList(m_repo + id);
	m_sizeOperation++;
}

void MainWindow :: deleteVersion(QString id, QString appid)
{
	m_sftpClient -> getDirList(m_repo + appid + "/" + id);
	m_sizeOperation++;
}

void MainWindow :: editVersion(QString id, QString zip, QString changelog, QString installer, QString appid)
{
	QString path = m_repo + appid + "/" + id + "/";
	if(zip != ""){
		SftpUploadParams p;
		p.localPath = zip;
		p.remotePath = path + (QFileInfo(zip).fileName());
		m_sftpClient -> uploadFile(p);
		m_sizeOperation++;
	}
	if(changelog != ""){
		SftpUploadParams p;
		p.localPath = changelog;
		p.remotePath = path + (QFileInfo(changelog).fileName());
		m_sftpClient -> uploadFile(p);
		m_sizeOperation++;
	}
	if(installer != ""){
		SftpUploadParams p;
		p.localPath = installer;
		p.remotePath = path + (QFileInfo(installer).fileName());
		m_sftpClient->uploadFile(p);
		m_sizeOperation++;
	}
}

void MainWindow :: editProg(QString id, QString desc, QString icon)
{
	QString path = m_repo + id + "/";
	if(desc != ""){
		SftpUploadParams p;
		p.localPath = desc;
		p.remotePath = path + (QFileInfo(desc).fileName());
		m_sftpClient->uploadFile(p);
		m_sizeOperation++;
	}
	if(icon != ""){
		SftpUploadParams p;
		p.localPath = icon;
		p.remotePath = path + (QFileInfo(icon).fileName());
		m_sftpClient->uploadFile(p);
		QFile::copy(p.localPath, QFileInfo(icon).fileName());
		m_sizeOperation++;
	}
}


