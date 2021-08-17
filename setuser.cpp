#include "setuser.h"
#include "ui_setuser.h"

SetUser::SetUser(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SetUser)
{
	ui->setupUi(this);
	setWindowTitle("Форма для редактирования пользователя");
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	ui->lineEdit_host->setText("22");
	ui->lineEdit_ip->setText("172.19.105.86");
	ui->lineEdit_repo->setText("/home/user/shared_dir/account1/");
	ui->lineEdit_user->setText("user");
}

SetUser::~SetUser()
{
	delete ui;
}

QString SetUser::getId()
{
	return ui->lineEdit_ip->text();
}

QString SetUser::getPort()
{
	return ui->lineEdit_host->text();
}

QString SetUser::getUser()
{
	return ui->lineEdit_user->text();
}

QString SetUser::getRepo()
{
	return ui->lineEdit_repo->text();
}

void SetUser::setLines(QString t_user, QString t_ip, QString t_port, QString t_repo)
{
	ui->lineEdit_host->setText(t_port);
	ui->lineEdit_ip->setText(t_ip);
	ui->lineEdit_repo->setText(t_repo);
	ui->lineEdit_user->setText(t_user);
}
