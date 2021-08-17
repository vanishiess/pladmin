#include "sftpsession.h"
#include "sftpparams.h"
#include <QFile>
#include <QByteArray>
#include <fcntl.h>
#include <QCoreApplication>

#define MAX_XFER_BUF_SIZE 16384


SftpSession::SftpSession()
{
	m_session = nullptr;
}

bool SftpSession::init(ssh_session sshSession)
{
	destroy();
	m_session = sftp_new(sshSession);
	if (!m_session) {
		return false;
	}
	int rc = sftp_init(m_session);
	if (rc != SSH_OK) {
		destroy();
		return false;
	}
	return true;
}

void SftpSession::destroy()
{
	if (m_session) {
		sftp_free(m_session);
		m_session = nullptr;
	}
}

void SftpSession::slotMkdir(const QString &path, mode_t mode)
{
	int rc = sftp_mkdir(m_session, qUtf8Printable(path), mode);
	emit sigMkdir(rc, path);
}

void SftpSession::slotRmdir(const QString &path)
{
	int rc = sftp_rmdir(m_session, qUtf8Printable(path));
	emit sigRmdir(rc, path);
}

void SftpSession::slotUnlink(const QString &path)
{
	int rc = sftp_unlink(m_session, qUtf8Printable(path));
	emit sigUnlink(rc, path);
}

void SftpSession::slotUpload(const SftpUploadParams &params)
{
	if (params.localPath.isEmpty() || params.remotePath.isEmpty()) {
		emit sigUpload(SSH_ERROR, params.localPath);
		return;
	}

	QFile srcFile(params.localPath);
	if (!srcFile.open(QFile::ReadOnly)) {
		emit sigUpload(SSH_ERROR, params.localPath);
		return;
	}

	sftp_file remFile = sftp_open(m_session,
								  qUtf8Printable(params.remotePath),
								  params.accessType,
								  params.mode);
	if (remFile == nullptr) {
		emit sigUpload(SSH_ERROR, params.localPath);
		return;
	}

	QByteArray buf;
	buf.resize(1024);
	bool txError = false;
	while (!srcFile.atEnd()) {
		ssize_t len = srcFile.read(buf.data(), 1024);
		if (len < 0) {
			txError = true;
			break;
		}
		ssize_t sent = sftp_write(remFile, buf.data(), len);
		if (sent != len) {
			txError = true;
			break;
		}
	}

	int rc = sftp_close(remFile);
	if (rc != SSH_OK) {
		txError = true;
	}

	emit sigUpload(txError ? SSH_ERROR : SSH_OK, params.localPath);
}

void SftpSession::slotDownload(const SftpDownloadParams &params)
{
	if (params.localPath.isEmpty() || params.remotePath.isEmpty()) {
		emit sigDownload(SSH_ERROR, params.remotePath);
		return;
	}

	QFile dstFile(params.localPath);
	if (!dstFile.open(QFile::WriteOnly)) {
		emit sigDownload(SSH_ERROR, params.remotePath);
		return;
	}
	sftp_file remFile = sftp_open(m_session,
								  qUtf8Printable(params.remotePath),
								  O_RDONLY,
								  0);
	if (remFile == nullptr) {
		emit sigDownload(SSH_ERROR, params.remotePath);
		return;
	}

	QByteArray buf;
	buf.resize(1024);
	bool txError = true;
	int nbytes, write, nwritten = 0, rc;
	char buffer[MAX_XFER_BUF_SIZE];
	while(txError == true) {
		nbytes = sftp_read(remFile, buffer, sizeof(buffer));
		if (nbytes == 0) {
			dstFile.close();
			break;
		} else if (nbytes < 0) {
			sftp_close(remFile);
			dstFile.close();
			txError = false;
		}
		write = dstFile.write(buffer, nbytes);
		if(write < 0){
			sftp_close(remFile);
			dstFile.close();
			txError = false;
		}
		nwritten += write;
	}
	dstFile.close();
	if(txError == true) {
		rc = sftp_close(remFile);
		if (rc != SSH_OK) {
			txError = false;
		}
	}
	emit sigDownload(txError ? SSH_OK : SSH_ERROR, params.remotePath);
}

void SftpSession::slotgetDirList(QString t_path)
{
	sftp_dir dir;
	sftp_attributes attributes;
	int rc;
	dir = sftp_opendir(m_session, t_path.toUtf8());
	if (!dir){
		emit sigGetDir(SSH_ERROR, {"0"}, {"0"});
		return;
	}
	QStringList list_name, list_type;
	QString name;
	while ((attributes = sftp_readdir(m_session, dir)) != NULL)
	{
		uint8_t type=0;
		name = attributes->name;
		if(name != "." && name != ".."){
			type = attributes->type;
			if(type == 2){
				slotgetDirList(t_path + "/" + attributes->name);
				slotRmdir(t_path + "/" + attributes->name);
			}else{
				slotUnlink(t_path + "/" + attributes->name);
			}
		}
		sftp_attributes_free(attributes);
	}
	slotRmdir(t_path);
	if (!sftp_dir_eof(dir)){
		emit sigGetDir(SSH_ERROR, list_name, list_type);
		return;
	}
	rc = sftp_closedir(dir);
	if (rc != SSH_OK){
		emit sigGetDir(SSH_ERROR, list_name, list_type);
		return;
	}
	emit sigGetDir(SSH_OK, list_name, list_type);
}
