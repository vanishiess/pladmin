
#include "sftpparams.h"
#include <fcntl.h>
#include <QMetaType>

struct SftpUploadParamsInitializer {
	SftpUploadParamsInitializer() {
		qRegisterMetaType<SftpUploadParams>("SftpUploadParams");
		qRegisterMetaType<SftpDownloadParams>("SftpDownloadParams");
	}
} g_sftpParamsInitializer;

SftpUploadParams::SftpUploadParams()
{
	accessType = O_WRONLY | O_CREAT | O_TRUNC;
	mode = S_IRWXU;
}

SftpDownloadParams::SftpDownloadParams()
{
//	accessType = O_RDONLY | O_WRONLY | O_CREAT;
//	mode = S_IRWXU;
}

