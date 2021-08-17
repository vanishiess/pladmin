#ifndef SFTPERRORS_H
#define SFTPERRORS_H

enum class ScpState {
	OK,
	UnknownError,
	ConnectFailed,
	UnknownServer,
	AuthFailed,
};

#endif // SFTPERRORS_H
