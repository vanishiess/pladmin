#ifndef SETVERSION_H
#define SETVERSION_H

#include <QDialog>
#include<prog.h>
#include<xml.h>
#include "moveprog.h"
#include "bin/inc/archive.h"
#include "bin/inc/archive_entry.h"

namespace Ui {
class setVersion;
}

class VersionList;

class setVersion : public QDialog
{
	Q_OBJECT

public:
	explicit setVersion(QWidget *parent = 0);
	~setVersion();
	void xml_replace(QVector<Prog> &m_mas_prog, int t_row,
					 QVector<MoveProg> &m_MoveProg, int t_rowVer, QVector<QStringList> &m_bufRemove);
	void xml_add_ver(QVector<Prog> &m_mas_prog, int t_row, QVector<MoveProg> &m_MoveProg);
	void setIdVer (QString t_idVer);
	void setChangelog(QString t_changelog);
	void setArchive(QString t_archive);
	void setVerType(QString t_type);
	void setDisplay(QString t_display);
	void setMainfile(QString t_mainfile);
	void setInstaller(QString t_installer);
	void setDate(QString t_date);
	void setAll(Version t_version);
	void setIdReadOnly(bool ok);
	void setFlags();

	QString getIdVer ();
	QString getChangelog();
	QString getArchive();
	QString getVerType();
	QString getDisplay();
	QString getMainfile();
	QString getInstaller();
	QString getDate();

private slots:
	void slotSetInstall();
	void slotSetZip();
	void slotSetChangelog();
	void slotExam();

private:
	struct archive_entry * m_entry;
	struct archive *m_archive;
	bool flagInstaller, flagChangelog, flagArchive;
	Ui::setVersion *ui;
	XML *m_xml;
	Prog *m_prog;
	VersionList *m_VersionList;
	QVector<Prog> *m_mas_prog;
	QVector<MoveProg> *m_MoveProg;
};

#endif // SETVERSION_H
