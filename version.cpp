#include "version.h"

Version::Version(){}

QString Version::getId()
{
	return m_id;
}

QString Version::getArchive()
{
	return m_archive;
}

QString Version::getType()
{
	return m_type;
}

QString Version::getMainfile()
{
	return m_mainfile;
}
QString Version::getInstaller()
{
	return m_installer;
}

QString Version::getChangelog()
{
	return m_changelog;
}

QString Version::getDate()
{
	return m_date;
}

QString Version::getDisplay()
{
	return m_display;
}

void Version::setArchive(QString t_archive)
{
	m_archive = t_archive;
}

void Version::setType(QString t_type)
{
	m_type = t_type;
}

void Version::setMainfile(QString t_mainfile)
{
	m_mainfile = t_mainfile;
}

void Version::setInstaller(QString t_installer)
{
	m_installer = t_installer;
}

void Version::setChangelog(QString t_changelog)
{
	m_changelog = t_changelog;
}

void Version::setDate(QString t_date)
{
	m_date = t_date;

}

void Version::setDisplay (QString t_display)
{
	m_display = t_display;

}

void Version::setId (QString t_id)
{
	m_id = t_id;

}

bool Version::operator == (const Version &v) const
{
	int notEq = 0;
	notEq += (v.m_archive != m_archive);
	notEq += (v.m_changelog != m_changelog);
	notEq += (v.m_date != m_date);
	notEq += (v.m_installer != m_installer);
	notEq += (v.m_mainfile != m_mainfile);
	notEq += (v.m_type != m_type);
	notEq += (v.m_display!=m_display);

	return !notEq;
}

bool Version::operator != (const Version &v) const
{
	return !(v == *this);
}
