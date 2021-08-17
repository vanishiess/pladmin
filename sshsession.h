#ifndef SSHSESSION_H
#define SSHSESSION_H

#include <QObject>
#include "libssh/libssh.h"

class SshSession : public QObject
{
	Q_OBJECT
public:
	SshSession();
	~SshSession();
	bool init();
	ssh_session getHandle() const;
	void setOption(ssh_options_e opt, const void *value);
	ssh_known_hosts_e isKnownServer();
	QString getHostKey();
	bool updateKnownHosts();

public slots:
	void slotConnectToHost();
	void slotDisconnectFromHost();
	void slotAuthorizeByKey();
	void slotAuthorizeByPassword(const QString &password);

signals:
	void sigConnected(int rc);
	void sigDisconnected();
	void sigAuthorizeByKey(int rc);
	void sigAuthorizeByPassword(int rc);

private:
	ssh_session m_session;

};

#endif // SSHSESSION_H
