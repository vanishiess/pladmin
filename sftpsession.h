#ifndef SFTPSESSION_H
#define SFTPSESSION_H

#include <QObject>
#include "libssh/libssh.h"
#include "libssh/sftp.h"
#include <sys/stat.h>

class SftpUploadParams;
class SftpDownloadParams;

class SftpSession : public QObject
{
	Q_OBJECT
public:
	SftpSession();
	bool init(ssh_session sshSession);
	void destroy();
	QStringList m_list;


public slots:
	void slotMkdir(const QString &path, mode_t mode);
	void slotRmdir(const QString &path);
	void slotUnlink(const QString &path);
	void slotUpload(const SftpUploadParams &params);
	void slotDownload(const SftpDownloadParams &params);
	void slotgetDirList(QString t_path);

signals:
	void sigMkdir(int rc, QString path);
	void sigRmdir(int rc, QString path );
	void sigUnlink(int rc, QString path);
	void sigUpload(int rc, QString localPath);
	void sigDownload(int rc, QString remotePath);
	void sigGetDir(int rc, QStringList list_name, QStringList list_type);

private:
	sftp_session m_session;
};

#endif // SFTPSESSION_H
