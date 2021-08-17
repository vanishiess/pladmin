#ifndef VERSION_H
#define VERSION_H

#include <QString>

class Version
{
private:
	QString m_id;
	QString m_archive;
	QString m_type;
	QString m_mainfile;
	QString m_installer;
	QString m_changelog;
	QString m_date;
	QString m_display;

public:
	Version();

	QString getId();
	QString getArchive();
	QString getType();
	QString getMainfile();
	QString getInstaller();
	QString getChangelog();
	QString getDate();
	QString getDisplay();

	void setArchive(QString t_archive);
	void setType(QString t_type);
	void setMainfile(QString t_mainfile);
	void setInstaller(QString t_installer);
	void setChangelog(QString t_changelog);
	void setDate(QString t_date);
	void setDisplay(QString t_display);
	void setId(QString t_id);

	bool operator == (const Version &v) const;
	bool operator != (const Version &v) const;
};

#endif // VERSION_H
