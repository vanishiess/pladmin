#include "prog.h"
#include <QString>

Prog::Prog(){}

QString Prog::getAppid()
{
	return m_appid;
}

QString Prog::getName()
{
	return m_name;
}

QString Prog::getIcon()
{
	return m_icon;
}

QString Prog::getDesc()
{
	return m_desc;
}

QString Prog::getType()
{
	return m_type;
}

QVector<Version> Prog::getVersions()
{
	return m_versions;
}

void Prog::setName(QString t_name)
{
	m_name = t_name;
}

void Prog::setAppid(QString t_appid)
{
	m_appid = t_appid;
}

void Prog::setIcon(QString t_icon)
{
	m_icon = t_icon;
}

void Prog::setDesc(QString t_desc)
{
	m_desc = t_desc;
}

void Prog::setType(QString t_type)
{
	m_type = t_type;
}

void Prog::setVersionInVector(Version t_version)
{
	m_versions.push_back(t_version);
}

void Prog::clearVersionInVector()
{
	m_versions.clear();
}

void Prog:: removeVersion(int i)
{
	m_versions.remove(i);
}

void Prog:: replaceVersion(int i, Version t_version)
{
	if(t_version.getArchive() != "")
		m_versions[i].setArchive(t_version.getArchive());
	if(t_version.getChangelog() != "")
		m_versions[i].setChangelog(t_version.getChangelog());
	if(t_version.getDate() != "")
		m_versions[i].setDate(t_version.getDate());
	if(t_version.getDisplay() != "")
		m_versions[i].setDisplay(t_version.getDisplay());
	if(t_version.getId() != "")
		m_versions[i].setId(t_version.getId());
	if(t_version.getInstaller() != "")
		m_versions[i].setInstaller(t_version.getInstaller());
	if(t_version.getMainfile() != "")
		m_versions[i].setMainfile(t_version.getMainfile());
	if(t_version.getType() != "")
		m_versions[i].setType(t_version.getType());
}

void Prog :: replaceProg(Prog t_prog)
{
	if(t_prog.getDesc() != "")
		m_desc = t_prog.getDesc();
	if(t_prog.getIcon() != "")
		m_icon = t_prog.getIcon();
	if(t_prog.getName() != "")
		m_name = t_prog.getName();
	if(t_prog.getType() != "")
		m_type = t_prog.getType();
}
