#ifndef USERLIST_H
#define USERLIST_H

#include <QDialog>
#include <QSettings>
#include <QVariant>
#include <QVector>
#include <QStandardItem>

namespace Ui {
class UserList;
}

class UserList : public QDialog
{
	Q_OBJECT

public:
	explicit UserList(QWidget *parent = 0);
	~UserList();
	struct User{
		QString ip;
		QString port;
		QString user;
		QString repo;
	};

	void addUser(User t_user);
	void removeUser();
	void setTable();
	void editUser(User t_user);

private slots:
	void slotAddUser();
	void slotDeleteUser();
	void slotAcceptUser();
	void slotEditUser();

private:
	Ui::UserList *ui;
	int m_size=1;
	QVector<User> m_users;
};

#endif // USERLIST_H
