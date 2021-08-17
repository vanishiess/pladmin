#include "userlist.h"
#include "ui_userlist.h"
#include "QStandardItem"
#include "setuser.h"
#include <QMessageBox>
#include <QStandardPaths>

UserList::UserList(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::UserList)
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle("Список пользователей");
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	path = path + "/" + "plAdmin2/users.ini";
	QSettings m_SettList(path, QSettings::IniFormat);
	QStringList groups = m_SettList.childGroups();
	if(!groups.isEmpty())
		groups.removeOne("main");
	for(const QString& group : groups){
		User user;

		m_SettList.beginGroup(group);
		user.user = group;
		user.ip = m_SettList.value("ip").toString();
		user.port = m_SettList.value("port").toString();
		user.repo = m_SettList.value("repo").toString();
		m_SettList.endGroup();

		m_users.push_back(user);
		m_size++;
	}
	connect(ui->btn_add,
			SIGNAL(clicked(bool)),
			SLOT(slotAddUser()));
	connect(ui->btn_delete,
			SIGNAL(clicked(bool)),
			SLOT(slotDeleteUser()));
	connect(ui->btn_accept,
			SIGNAL(clicked(bool)),
			SLOT(slotAcceptUser()));
	connect(ui->tableView,
			SIGNAL(doubleClicked(QModelIndex)),
			SLOT(slotAcceptUser()));
	connect(ui->btn_edit,
			SIGNAL(clicked(bool)),
			SLOT(slotEditUser()));
	setTable();
}

UserList::~UserList()
{
	delete ui;
}

void UserList :: addUser(User t_user)
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	path = path + "/" + "plAdmin2/users.ini";
	QSettings m_SettList(path, QSettings::IniFormat);
	m_SettList.beginGroup("main");
	if(m_SettList.value("count", 0).toInt()>0)
		m_SettList.setValue("count", ++m_size);
	else
		m_SettList.setValue("count", 1);
	m_SettList.endGroup();

	m_users.push_back(t_user);
	m_SettList.beginGroup(t_user.user);
	m_SettList.setValue("ip", t_user.ip);
	m_SettList.setValue("port", t_user.port);
	m_SettList.setValue("repo", t_user.repo);
	m_SettList.endGroup();
}

void UserList :: removeUser()
{
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){								//пока работает только
		int row = selectModel -> selectedRows().first().row();		//с одной выделенной строкой
		QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
		path = path + "/" + "plAdmin2/users.ini";
		QSettings m_SettList(path , QSettings::IniFormat);
		m_SettList.remove(m_users[row].user);
		m_users.remove(row);
		m_SettList.setValue("main/count", --m_size);
	}
}

void UserList :: setTable()
{
	QStandardItemModel *model = new QStandardItemModel;
	QStandardItem *item;
	QStringList horizontalHeader;
	horizontalHeader.append("Пользователь");
	horizontalHeader.append("Ip");
	horizontalHeader.append("Порт");
	horizontalHeader.append("Путь к репо");
	model -> setHorizontalHeaderLabels(horizontalHeader);
	for(int i = 0; i < m_users.size(); i++){
		item = new QStandardItem(m_users[i].user);
		model -> setItem(i, 0, item);
		item = new QStandardItem(m_users[i].ip);
		model -> setItem(i, 1, item);
		item = new QStandardItem(m_users[i].port);
		model -> setItem(i, 2, item);
		item = new QStandardItem(m_users[i].repo);
		model -> setItem(i, 3, item);
	}

	ui -> tableView -> setModel(model);
	ui -> tableView -> resizeColumnsToContents();
	ui -> tableView -> resizeRowsToContents();
}

void UserList :: slotAddUser()
{
	SetUser m_SetUser;
	if(m_SetUser.exec() == QDialog::Accepted){
		User user;
		user.ip = m_SetUser.getId();
		user.port = m_SetUser.getPort();
		user.user = m_SetUser.getUser();
		user.repo = m_SetUser.getRepo();
		this->addUser(user);
		setTable();
	}
}

void UserList :: slotDeleteUser()
{
	this -> removeUser();
	setTable();
}

void UserList :: slotAcceptUser()
{
	QMessageBox msg;
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){
		int row = selectModel -> selectedRows().first().row();
		QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
		path = path + "/" + "plAdmin2/users.ini";
		QSettings m_SettList(path, QSettings::IniFormat);
		m_SettList.setValue("main/accepted", m_users[row].user);
		msg.setText("Пользователь выбран");
		msg.exec();
		this->close();
	} else{
		msg.setText("Пользователь не выбран");
		msg.exec();
	}
}

void UserList :: editUser(User t_user)
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation);
	path = path + "/" + "plAdmin2/users.ini";
	QSettings m_SettList(path, QSettings::IniFormat);

	m_SettList.beginGroup(t_user.user);
	m_SettList.setValue("ip", t_user.ip);
	m_SettList.setValue("port", t_user.port);
	m_SettList.setValue("repo", t_user.repo);
	m_SettList.endGroup();
}

void UserList :: slotEditUser()
{
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){								//пока работает только
		int row = selectModel -> selectedRows().first().row();
		SetUser m_SetUser;
		m_SetUser.setLines(m_users[row].user, m_users[row].ip, m_users[row].port, m_users[row].repo);
		if(m_SetUser.exec() == QDialog::Accepted){
			m_users[row].ip = m_SetUser.getId();
			m_users[row].port = m_SetUser.getPort();
			m_users[row].repo = m_SetUser.getRepo();
			m_users[row].user = m_SetUser.getUser();

			User user;
			user.ip = m_SetUser.getId();
			user.port = m_SetUser.getPort();
			user.user = m_SetUser.getUser();
			user.repo = m_SetUser.getRepo();
			this->editUser(user);
			setTable();
		}
	}
}
