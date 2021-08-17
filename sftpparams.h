#ifndef SFTPPARAMS_H
#define SFTPPARAMS_H

#include <QString>
#include <sys/stat.h>
#include "libssh/sftp.h"

struct SftpUploadParams
{
	SftpUploadParams();

	int accessType;
	mode_t mode;
    QString localPath;
	QString remotePath;
};

struct SftpDownloadParams
{
	SftpDownloadParams();

//    int accessType;
//    mode_t mode;
	QString localPath;
	QString remotePath;
};

#endif // SFTPPARAMS_H
