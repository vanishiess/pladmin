#ifndef SFTPCLIENT_H
#define SFTPCLIENT_H

#include <QObject>
#include <QThread>
#include <sys/stat.h>

class SshSession;
class SftpSession;
class SftpUploadParams;
class SftpDownloadParams;

class SftpClient : public QObject
{
	Q_OBJECT
public:
	explicit SftpClient(QObject *parent = nullptr);
	~SftpClient();
	void setAddr(const QString &addr);
	void setUsername(const QString &username);
	void setPort(int port);
	void connectToHost();
	void disconnectFromHost();
	void confirmServerKey();
	void providePassword(const QString &password);
	void mkdir(const QString &path, mode_t mode = S_IRWXU);
	void rmdir(const QString &path);
	void unlink(const QString &path);
	void uploadFile(const SftpUploadParams &params);
	void downloadFile(const SftpDownloadParams &params);
    void getDirList(QString t_path);

signals:
	void sigConnectToHost(bool ok);
	void sigDisconnectFromHost();
	void sigConfirmServerKey(const QString &sha1);
	void sigNeedPassword();
	void sigMkdir(bool ok, QString path);
	void sigRmdir(bool ok, QString path);
	void sigUnlink(bool ok, QString path);
	void sigUploadFile(bool ok, QString localPath);
	void sigDownloadFile(bool ok, QString remotePath);
	void sigXMLDownload(bool ok);
	void sigGetDir(bool ok, QStringList list_name, QStringList list_type);

	// служебные сигналы, для внутренних сообщений
	void _sigConnectToHost();
	void _sigDisconnectFromHost();
	void _sigAuthorizeByKey();
	void _sigAuthorizeByPassword(const QString &password);
	void _sigMkdir(const QString &path, mode_t mode);
	void _sigRmdir(const QString &path);
	void _sigUnlink(const QString &path);
    void _sigUploadFile(const SftpUploadParams &params);
    void _sigDownloadFile(const SftpDownloadParams &params);
    void _sigGetDirList(const QString &path);

private slots:
	void _slotConnectToHost(int rc);
	void _slotDisconnectFromHost();
	void _slotAuthorize(int rc);
	void _slotMkdir(int rc, QString path);
	void _slotRmdir(int rc, QString path);
	void _slotUnlink(int rc, QString path);
	void _slotUploadFile(int rc, QString localPath);
	void _slotDownloadFile(int rc, QString remotePath);
	void _slotGetDir(int rc, QStringList list_name, QStringList list_type);

private:
	enum class State {
		Idle,
		Connecting,
		Disconnecting,
		CheckServerKey,
		AuthByKey,
		AuthAskPassword,
		AuthByPassword
	};

	QThread m_thread;
	SshSession *m_sshSession;
	SftpSession *m_sftpSession;
	State m_state;

	QString m_host;
	QString m_username;
	int m_port;
	QString m_password;

	void doAuth();
};

#endif // SFTPCLIENT_H
