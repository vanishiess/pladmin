#include "sshsession.h"
#include <QDebug>

SshSession::SshSession()
{
	m_session = nullptr;
}

SshSession::~SshSession()
{
	if (m_session) {
		ssh_free(m_session);
	}
}

bool SshSession::init()
{
	if (m_session) {
		ssh_free(m_session);
	}
	m_session = ssh_new();
	return (m_session != nullptr);
}

ssh_session SshSession::getHandle() const
{
	return m_session;
}

void SshSession::setOption(ssh_options_e opt, const void *value)
{
	ssh_options_set(m_session, opt, value);
}

ssh_known_hosts_e SshSession::isKnownServer()
{
	return ssh_session_is_known_server(m_session);
}

QString SshSession::getHostKey()
{
	unsigned char *hash = nullptr;
	size_t hlen = 0;
	ssh_key srv_pubkey = nullptr;

	int rc = ssh_get_server_publickey(m_session, &srv_pubkey);
	if (rc < 0) {
		return QString();
	}

	rc = ssh_get_publickey_hash(srv_pubkey,
								SSH_PUBLICKEY_HASH_SHA1,
								&hash,
								&hlen);
	ssh_key_free(srv_pubkey);
	if (rc < 0) {
		return QString();
	}

	char *hexa = ssh_get_hexa(hash, hlen);
	QString keyStr(hexa);
	ssh_string_free_char(hexa);
	ssh_clean_pubkey_hash(&hash);

	return keyStr;
}

bool SshSession::updateKnownHosts()
{
	int rc = ssh_session_update_known_hosts(m_session);
	return (rc == SSH_OK);
}

void SshSession::slotConnectToHost()
{
	int rc = ssh_connect(m_session);
	emit sigConnected(rc);
}

void SshSession::slotDisconnectFromHost()
{
	ssh_disconnect(m_session);
	emit sigDisconnected();
}

void SshSession::slotAuthorizeByKey()
{
	int rc = ssh_userauth_publickey_auto(m_session, nullptr, nullptr);
	emit sigAuthorizeByKey(rc);
}

void SshSession::slotAuthorizeByPassword(const QString &password)
{
	int rc = ssh_userauth_password(m_session, nullptr, qUtf8Printable(password));
	emit sigAuthorizeByPassword(rc);
}
