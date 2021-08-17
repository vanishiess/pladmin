#include "sftpclient.h"
#include "sshsession.h"
#include "sftpsession.h"
#include <QDebug>

struct SftpClientInitializer {
	SftpClientInitializer() {
		qRegisterMetaType<mode_t>("mode_t");
	}
} g_sftpInitializer;

SftpClient::SftpClient(QObject *parent) :
	QObject(parent)
{
	m_port = 22;
	m_state = State::Idle;
	m_sshSession = new SshSession();
	m_sshSession->moveToThread(&m_thread);
	m_sftpSession = new SftpSession();
	m_sftpSession->moveToThread(&m_thread);

	// connectToHost
	connect(this,
			SIGNAL(_sigConnectToHost()),
			m_sshSession,
			SLOT(slotConnectToHost()));
	connect(m_sshSession,
			SIGNAL(sigConnected(int)),
			this,
			SLOT(_slotConnectToHost(int)));
	// disconnectFromHost
	connect(this,
			SIGNAL(_sigDisconnectFromHost()),
			m_sshSession,
			SLOT(slotDisconnectFromHost()));
	connect(m_sshSession,
			SIGNAL(sigDisconnected()),
			this,
			SLOT(_slotDisconnectFromHost()));
	// authorizeByKey
	connect(this,
			SIGNAL(_sigAuthorizeByKey()),
			m_sshSession,
			SLOT(slotAuthorizeByKey()));
	connect(m_sshSession,
			SIGNAL(sigAuthorizeByKey(int)),
			this,
			SLOT(_slotAuthorize(int)));
	// authorizeByPassword
	connect(this,
			SIGNAL(_sigAuthorizeByPassword(QString)),
			m_sshSession,
			SLOT(slotAuthorizeByPassword(QString)));
	connect(m_sshSession,
			SIGNAL(sigAuthorizeByPassword(int)),
			this,
			SLOT(_slotAuthorize(int)));
	// mkdir
	connect(this,
			SIGNAL(_sigMkdir(QString,mode_t)),
			m_sftpSession,
			SLOT(slotMkdir(QString,mode_t)));
	connect(m_sftpSession,
			SIGNAL(sigMkdir(int, QString)),
			this,
			SLOT(_slotMkdir(int, QString)));
	// rmdir
	connect(this,
			SIGNAL(_sigRmdir(QString)),
			m_sftpSession,
			SLOT(slotRmdir(QString)));
	connect(m_sftpSession,
			SIGNAL(sigRmdir(int, QString)),
			this,
			SLOT(_slotRmdir(int, QString)));
	// unlink
	connect(this,
			SIGNAL(_sigUnlink(QString)),
			m_sftpSession,
			SLOT(slotUnlink(QString)));
	connect(m_sftpSession,
			SIGNAL(sigUnlink(int, QString)),
			this,
			SLOT(_slotUnlink(int, QString)));
	// upload
	connect(this,
			SIGNAL(_sigUploadFile(SftpUploadParams)),
			m_sftpSession,
			SLOT(slotUpload(SftpUploadParams)));
	connect(m_sftpSession,
			SIGNAL(sigUpload(int, QString)),
			this,
			SLOT(_slotUploadFile(int, QString)));
	// download
	connect(this,
			SIGNAL(_sigDownloadFile(SftpDownloadParams)),
			m_sftpSession,
			SLOT(slotDownload(SftpDownloadParams)));
	connect(m_sftpSession,
			SIGNAL(sigDownload(int, QString)),
			this,
			SLOT(_slotDownloadFile(int, QString)));
	// getdir
	connect(this,
			SIGNAL(_sigGetDirList(QString)),
			m_sftpSession,
			SLOT(slotgetDirList(QString)));
	connect(m_sftpSession,
			SIGNAL(sigGetDir(int, QStringList, QStringList)),
			this,
			SLOT(_slotGetDir(int, QStringList, QStringList)));



	m_thread.start();
}

SftpClient::~SftpClient()
{
	m_thread.quit();
	m_thread.wait();

	if (m_sftpSession) {
		delete m_sftpSession;
	}

	if (m_sshSession) {
		delete m_sshSession;
	}
}

void SftpClient::setAddr(const QString &addr)
{
	m_host = addr;
	m_host.detach();
}

void SftpClient::setUsername(const QString &username)
{
	m_username = username;
	m_username.detach();
}

void SftpClient::setPort(int port)
{
	m_port = port;
}

void SftpClient::connectToHost()
{
	m_state = State::Connecting;
	bool ok = m_sshSession->init();
	if (!ok) {
		emit sigConnectToHost(false);
	}
	m_sshSession->setOption(SSH_OPTIONS_HOST, qUtf8Printable(m_host));
	m_sshSession->setOption(SSH_OPTIONS_USER, qUtf8Printable(m_username));
	m_sshSession->setOption(SSH_OPTIONS_PORT, &m_port);
	emit _sigConnectToHost();
}

void SftpClient::disconnectFromHost()
{
	m_state = State::Disconnecting;
	m_sftpSession->destroy();
	emit _sigDisconnectFromHost();
}

void SftpClient::confirmServerKey()
{
	// пользователь посмотрел ключ, и говорит что всё норм
	// сохраняем сервер в known_hosts
	if (!m_sshSession->updateKnownHosts()) {
		m_state = State::Idle;
		emit sigConnectToHost(false);
		return;
	}
	// переходим к авторизации
	m_state = State::AuthByKey;
	doAuth();
}

void SftpClient::providePassword(const QString &password)
{
	m_password = password;
	m_state = State::AuthByPassword;
	doAuth();
}

void SftpClient::mkdir(const QString &path, mode_t mode)
{
	emit _sigMkdir(path, mode);
}

void SftpClient::rmdir(const QString &path)
{
	emit _sigRmdir(path);
}

void SftpClient::unlink(const QString &path)
{
	emit _sigUnlink(path);
}

void SftpClient::uploadFile(const SftpUploadParams &params)
{
	emit _sigUploadFile(params);
}

void SftpClient::downloadFile(const SftpDownloadParams &params)
{
	emit _sigDownloadFile(params);
}

void SftpClient::getDirList(const QString t_path)
{
	emit _sigGetDirList(t_path);
}


void SftpClient::_slotConnectToHost(int rc)
{
	qDebug() << "connect to host" << rc;

	if (rc != SSH_OK) {
		m_state = State::Idle;
		emit sigConnectToHost(false);
		return;
	}

	// проверяем сервер
	ssh_known_hosts_e khe = m_sshSession->isKnownServer();
	switch (khe) {
	case SSH_KNOWN_HOSTS_OK:
		// сервер настоящий, начинаем авторизацию
		m_state = State::AuthByKey;
		doAuth();
		return;

	case SSH_KNOWN_HOSTS_UNKNOWN: // неизвестный сервер
	case SSH_KNOWN_HOSTS_CHANGED: // ключ поменялся
	case SSH_KNOWN_HOSTS_NOT_FOUND: // known_hosts не найден
		// ждём подтверждение от пользователя
		m_state = State::CheckServerKey;
		emit sigConfirmServerKey(m_sshSession->getHostKey());
		return;

	default:
		m_state = State::Idle;
		emit sigConnectToHost(false);
		return;
	}
}

void SftpClient::_slotDisconnectFromHost()
{
	emit sigDisconnectFromHost();
}

void SftpClient::_slotAuthorize(int rc)
{
	bool ok = (rc == SSH_AUTH_SUCCESS);

	switch (m_state) {
	case State::AuthByKey:
		qDebug() << "key auth finished" << (ok ? "OK" : "Error");
		if (!ok) {
			// не проканало, пробуем с паролем
			m_state = State::AuthAskPassword;
			doAuth();
			return;
		}
		break;

	case State::AuthByPassword:
		qDebug() << "pass auth finished" << (ok ? "OK" : "Error");
		if (!ok) {
			// не проканало, юзер идёт на*уй
			m_state = State::Idle;
			emit sigConnectToHost(false);
			disconnectFromHost();
			return;
		}
		break;

	default:
		break;
	}

	qDebug() << "auth successfull!";
	if (!m_sftpSession->init(m_sshSession->getHandle())) {
		m_state = State::Idle;
		emit sigConnectToHost(false);
		disconnectFromHost();
		return;
	}
	qDebug() << "sftp session created";
	emit sigConnectToHost(true);
}

void SftpClient::_slotMkdir(int rc, QString path)
{
	bool ok = (rc == SSH_OK);
	emit sigMkdir(ok, path);
}

void SftpClient::_slotRmdir(int rc, QString path)
{
	bool ok = (rc == SSH_OK);
	emit sigRmdir(ok, path);
}

void SftpClient::_slotUnlink(int rc, QString path)
{
	bool ok = (rc == SSH_OK);
	emit sigUnlink(ok, path);
}

void SftpClient::_slotUploadFile(int rc, QString localPath)
{
	bool ok = (rc == SSH_OK);
	emit sigUploadFile(ok, localPath);
}

void SftpClient::_slotDownloadFile(int rc, QString remotePath)
{
	bool ok = (rc == SSH_OK);
	emit sigDownloadFile(ok, remotePath);
}

void SftpClient::_slotGetDir(int rc, QStringList list_name, QStringList list_type)
{
	bool ok = (rc == SSH_OK);
	emit sigGetDir(ok, list_name, list_type);
}

void SftpClient::doAuth()
{
	switch (m_state) {
	case State::AuthByKey: // сначала авторизация по ключу
		emit _sigAuthorizeByKey();
		break;

	case State::AuthAskPassword:
		emit sigNeedPassword();
		break;

	case State::AuthByPassword: // затем уже по паролю
		emit _sigAuthorizeByPassword(m_password);
		break;

	default:
		return;
	}
}



