#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sftpclient.h"
#include "setprog.h"
#include "xml.h"
#include "userlist.h"
#include "versionlist.h"
#include "moveprog.h"
#include "loading.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SftpUploadParams;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();
	void SetTable();
	void SetVersionTable(int row);
	void getDirList(QString t_path);

	void addProg(QString id, QString desc, QString icon);
	void addProg(QString id, QString desc);					//прога без иконки
	void addVersion(QString id, QString zip, QString changelog, QString installer, QString appid);
	void deleteProg(QString id);
	void deleteVersion(QString id, QString appid);
	void editVersion(QString id, QString zip, QString changelog, QString installer, QString appid);
	void editProg(QString id, QString desc, QString icon);

private slots:
	void slotConnectButton();
	void slotConnected(bool ok);
	void slotDisconnected();
	void slotCheckServerKey(const QString &sha1);
	void slotNeedPassword();
	void slotXMLbutton();

	void slotMkdir(bool ok, QString path);
	void slotRmdir(bool ok, QString path);
	void slotRemove(bool ok, QString path);
	void slotUpload(bool ok, QString localPath);
	void slotDownload(bool ok, QString remotePath);
	void slotGetdir(bool ok, QStringList list_name, QStringList list_type);
	void slotUsersList();
	void slotpProgSelect(const QModelIndex & index );
	void slotAddProg();
	void slotEditProg();
	void slotDeleteProg();
	void slotSaveButton();

private:
	int m_sizeOperation = 0;				// счётчик для диапазона прогрессбара
	Loading *m_Loading;
	Ui::MainWindow *ui;
	SftpClient *m_sftpClient;
	UserList *m_UserList;
	QVector<Prog> m_QVectorProg;
	QVector<MoveProg> m_moveProg;
	QVector<QStringList> m_buff_remove;		//запоминает элементы, которые надо удалить
	VersionList *m_VersionList;
	bool m_connected = false;
	QString m_host;
	QString m_user;
	QString m_password;
	QString m_repo;
	QString m_ProgAccepted;
	int m_port;


};
#endif // MAINWINDOW_H
