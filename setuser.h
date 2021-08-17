#ifndef SETUSER_H
#define SETUSER_H

#include "userlist.h"
#include <QDialog>

namespace Ui {
class SetUser;
}

class SetUser : public QDialog
{
	Q_OBJECT

public:
	explicit SetUser(QWidget *parent = 0);
	~SetUser();
	QString getId();
	QString getPort();
	QString getUser();
	QString getRepo();
	void setLines(QString t_user, QString t_ip, QString t_port, QString t_repo);

private:
	Ui::SetUser *ui;
};

#endif // SETUSER_H
