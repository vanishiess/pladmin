#ifndef PROG_H
#define PROG_H

#include <QString>
#include <QVector>
#include "version.h"

class Prog
{
private:
	QString m_appid;
	QString m_name;
	QString m_icon;
	QString m_desc;
	QString m_type;
	QVector<Version> m_versions;

public:
	Prog();
	QString getAppid();
	QString getName();
	QString getIcon();
	QString getDesc();
	QString getType();
	QVector<Version> getVersions();


	void replaceVersion(int i, Version t_version);
	void removeVersion(int i);
	void setAppid(QString t_appid);
	void setName(QString t_name);
	void setIcon(QString t_icon);
	void setDesc(QString t_desc);
	void setType(QString t_type);
	void setVersionInVector(Version t_version);
	void clearVersionInVector();
	void replaceProg(Prog t_prog);


};

#endif // PROG_H
