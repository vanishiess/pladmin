#include "archive.h"
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QTextCodec>


Archive::Archive()
{
	m_archive = NULL;
	m_type = Unknown;
}

Archive::Archive(const QString &name)
{
	m_name = name;
	m_archive = NULL;
	m_type = Unknown;
}

Archive::~Archive()
{
	close();
	// подчищаем за собой
	cleanUp();
}

bool Archive::open(OpenMode mode)
{
	int r;
	m_mode = mode;

	prepareOperation();

	if (isOpen()) {
		setErrorStr(QObject::tr("Archive is already open!"));
		return false;
	}
	if (m_name.isEmpty()) {
		setErrorStr(QObject::tr("Filename is not specified"));
		return false;
	}

	m_file.setFileName(m_name);
	if (!m_file.open(QFile::ReadOnly)) {
		setErrorStr(QObject::tr("Can't open file %1").arg(m_name));
		return false;
	}

	switch (m_mode) {
	case Read:
		m_archive = archive_read_new();
		archive_read_support_filter_all(m_archive);
		archive_read_support_format_all(m_archive);
		r = archive_read_open_fd(m_archive, m_file.handle(), 10240);
		break;

	default:
		setErrorStr(QObject::tr("Wrong open mode"));
		return false;
	}
	if (r != ARCHIVE_OK) {
		setErrorStr(archive_error_string(m_archive));
		close();
		return false;
	}
	m_type = Unknown;
	return true;
}

bool Archive::open(const QString &name, Archive::OpenMode mode)
{
	m_name = name;
	return open(mode);
}

void Archive::close()
{
	if (!m_archive) {
		return;
	}

	switch (m_mode) {
	case Read:
		archive_read_close(m_archive);
		archive_read_free(m_archive);
		break;

	case Write:
		archive_write_close(m_archive);
		archive_write_free(m_archive);
		break;
	}

	m_file.close();
	m_archive = NULL;
	m_entry = NULL;
}

bool Archive::isOpen() const
{
	return (bool)m_archive;
}

void Archive::setType(Archive::Type t)
{
	switch (t) {
	case Unknown:
	case TarZipped:
		// можно добавлять
		m_type = t;
		break;

	default:
		m_type = Unknown;
	}
}

Archive::Type Archive::getType() const
{
	return m_type;
}

QString Archive::getErrorStr() const
{
	return m_errorStr;
}

void Archive::cleanUp()
{
	// пальба по ногам, можно удалять файлы из под себя, если не осторожно
	foreach (const QFileInfo & fi, m_tempFiles) {
		QFile::remove(fi.absoluteFilePath());
	}
	m_tempFiles.clear();
}

void Archive::setErrorStr(const QString &str)
{
	m_errorStr = str;
}

void Archive::clearError()
{
	m_errorStr.clear();
}

void Archive::prepareOperation()
{
	m_lastUnpackedFiles.clear();
	clearError();
}

bool Archive::extract(const QString &dest, Type t)
{
	m_type = t;
	QDir d;
	ssize_t size;
	bool error = false;
	QString archiveName = m_name;
	QTextCodec *cp866 = QTextCodec::codecForName("CP866");

	prepareOperation();

	if (QFileInfo(dest).isFile()) {
		setErrorStr(QObject::tr("Destination is file"));
		return false;
	}

	if (!isOpen()) {
		setErrorStr(QObject::tr("Archive is not open"));
		return false;
	}

	switch (m_type) {
	case TarZipped: {
		// сперва надо распаковать tar в временную папку
		Archive a(m_name);
		if (!a.open(Archive::Read)) {
			setErrorStr(a.getErrorStr());
			return false;
		}
		a.extract(QDir::tempPath());
		archiveName = a.getLastUnpackedFilesInfo().at(0).absoluteFilePath();
		// закрываем текущий архив, переоткрываем на распакованный tar
		close();
		if (!open(archiveName, Read)) {
			return false;
		}
		// временный файл на удаление
		m_tempFiles << QFileInfo(archiveName);
	}
	// fall through
	// сразу же распаковываем обычный tar

	case Unknown:
		while (archive_read_next_header(m_archive, &m_entry) == ARCHIVE_OK) {
			// борьба с кодировками
			QString entryPath = dest + "/";
			const char *pathAscii = archive_entry_pathname(m_entry);
			const wchar_t *pathWide = archive_entry_pathname_w(m_entry);
			if (pathAscii) {
				entryPath += cp866->toUnicode(pathAscii);
			} else if (pathWide) {
				entryPath += QString::fromWCharArray(pathWide);
			} else {
				// поражение, пропускаем файл
				continue;
			}
			QFileInfo destFileInfo(entryPath);
			// создаем папку назначения
			if (!d.mkpath(destFileInfo.absolutePath())) {
				error = true;
				setErrorStr(QObject::tr("Can't create output subdir"));
				continue;
			}
			if (destFileInfo.isDir()) {
				// папки пропускаем
				continue;
			}
			// открываем файл для записи
			QFile destFile(destFileInfo.absoluteFilePath());
			if (!destFile.open(QFile::WriteOnly)) {
				setErrorStr(QObject::tr("Can't open file %1 for write").arg(destFile.fileName()));
				error = true;
				continue;
			}
			while (1) {
				size = archive_read_data(m_archive, m_buf, sizeof(m_buf));
				if (size < 0) {
					// произошла ошибка
					setErrorStr(QString(archive_error_string(m_archive)));
					error = true;
					break;
				}
				if (size == 0) {
					// достигнут конец файла
					break;
				}
				destFile.write(m_buf, size);
			}
			destFile.close();
			m_lastUnpackedFiles << destFileInfo;
		}
		break;

	default:
		return false;

	}

	return !error;
}

QFileInfoList Archive::getLastUnpackedFilesInfo() const
{
	return m_lastUnpackedFiles;
}

uint64_t Archive::contentSize(const QString &fname)
{
	uint64_t				size = 0;
	struct archive          *archive;
	struct archive_entry    *entry;
	QFile					file;

	if (fname.isEmpty()) {
		return 0;
	}
	file.setFileName(fname);
	if (!file.open(QFile::ReadOnly)) {
		return 0;
	}

	archive = archive_read_new();
	archive_read_support_filter_all(archive);
	archive_read_support_format_all(archive);
	archive_read_open_fd(archive, file.handle(), 10240);

	while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
		size += archive_entry_size(entry);
	}

	archive_read_close(archive);
	archive_read_free(archive);

	return size;
}
