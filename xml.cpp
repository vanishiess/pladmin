#include <xml.h>
#include "sftpparams.h"
#include <QtXml>
#include <QFile>

XML::XML(){}

void XML::editXML(QVector<Prog> t_mas_prog)
{
	QDomDocument doc;
	doc.appendChild(doc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
	QDomNode progs = doc.createElement(QString("progs"));
	for(int i = 0; i < t_mas_prog.size(); i++){
		QDomNode prog = doc.createElement(QString("prog"));
		QDomElement prog_child = doc.createElement(QString("appid"));
		QDomText text=doc.createTextNode(t_mas_prog[i].getAppid());
		prog_child.appendChild(text);
		prog.appendChild(prog_child);

		prog_child = doc.createElement(QString("name"));
		text=doc.createTextNode(t_mas_prog[i].getName());
		prog_child.appendChild(text);
		prog.appendChild(prog_child);

		prog_child = doc.createElement(QString("icon"));
		text=doc.createTextNode(t_mas_prog[i].getIcon());
		prog_child.appendChild(text);
		prog.appendChild(prog_child);

		prog_child = doc.createElement(QString("desc"));
		text=doc.createTextNode(t_mas_prog[i].getDesc());
		prog_child.appendChild(text);
		prog.appendChild(prog_child);

		prog_child = doc.createElement(QString("type"));
		text=doc.createTextNode(t_mas_prog[i].getType());
		prog_child.appendChild(text);
		prog.appendChild(prog_child);

		prog_child = doc.createElement(QString("versions"));
		QVector<Version> m_versions=t_mas_prog[i].getVersions();
		for(int j = 0; j < m_versions.size(); j++){
			QDomElement version_child = doc.createElement(QString("version"));
			version_child.setAttribute("id", m_versions[j].getId());

			QDomElement version_child_at = doc.createElement(QString("changelog"));
			QDomText text=doc.createTextNode(m_versions[j].getChangelog());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);

			version_child_at = doc.createElement(QString("archive"));
			text=doc.createTextNode(m_versions[j].getArchive());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);

			version_child_at = doc.createElement(QString("type"));
			version_child_at.setNodeValue(m_versions[j].getType());
			text=doc.createTextNode(m_versions[j].getType());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);

			version_child_at = doc.createElement(QString("mainfile"));
			version_child_at.setNodeValue(m_versions[j].getMainfile());
			text=doc.createTextNode(m_versions[j].getMainfile());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);

			version_child_at = doc.createElement(QString("installer"));
			version_child_at.setNodeValue(m_versions[j].getInstaller());
			text=doc.createTextNode(m_versions[j].getInstaller());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);

			version_child_at = doc.createElement(QString("date"));
			version_child_at.setNodeValue(m_versions[j].getDate());
			text=doc.createTextNode(m_versions[j].getDate());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);

			version_child_at = doc.createElement(QString("display"));
			version_child_at.setNodeValue(m_versions[j].getDisplay());
			text=doc.createTextNode(m_versions[j].getDisplay());
			version_child_at.appendChild(text);
			version_child.appendChild(version_child_at);
			prog_child.appendChild(version_child);
		}
		prog.appendChild(prog_child);
		progs.appendChild(prog);
	}
	doc.appendChild(progs);

	QFile file_save("repo2.xml");
	file_save.open(QFile::WriteOnly);

	QTextStream out(&file_save);
	doc.save(out, 3);
	file_save.close();
}


void XML::readXML(QVector<Prog> &t_mas_prog)
{
	t_mas_prog.clear();
	QFile file("repo2.xml");

	file.open(QFile::ReadOnly);
	Version m_version;
	QDomDocument doc;
	QString error;

	int line, column;

	if (!doc.setContent(&file, &error, &line, &column)) {
		qDebug() << "Error:" << error << "in line " << line << "column" << column;
		return ;
	}
	QDomNode prog = doc.firstChildElement("progs");
	prog = prog.firstChild();
	while (!prog.isNull()) {						//записываем поля prog
		if (prog.isElement()) {
			Prog m_prog;
			QDomElement element = prog.toElement();
			element = element.firstChildElement();
			m_prog.setAppid(element.firstChild().nodeValue());
			element = element.nextSiblingElement();
			m_prog.setName(element.firstChild().nodeValue());
			element = element.nextSiblingElement();
			if(element.nodeName()=="icon"){				//у прошивок нет этого поля
				m_prog.setIcon(element.firstChild().nodeValue());
				element = element.nextSiblingElement();
			}
			m_prog.setDesc(element.firstChild().nodeValue());
			element = element.nextSiblingElement();
			m_prog.setType(element.firstChild().nodeValue());
			element = element.nextSiblingElement();
			element=element.firstChildElement();
			QDomElement version = element;								//записываем поля version
			while(!version.isNull()){									//в вектор версий
				m_version.setId(element.attributeNode("id").value());
				version = version.firstChildElement();
				m_version.setChangelog(version.firstChild().nodeValue());
				version = version.nextSiblingElement();
				m_version.setArchive(version.firstChild().nodeValue());
				version = version.nextSiblingElement();
				m_version.setType(version.firstChild().nodeValue());
				version = version.nextSiblingElement();
				m_version.setMainfile(version.firstChild().nodeValue());
				version = version.nextSiblingElement();
				m_version.setInstaller(version.firstChild().nodeValue());
				version = version.nextSiblingElement();
				m_version.setDate(version.firstChild().nodeValue());
				version = version.nextSiblingElement();
				m_version.setDisplay(version.firstChild().nodeValue());
				m_prog.setVersionInVector(m_version);
				element = element.nextSiblingElement();
				version=element;
			}
			t_mas_prog.push_back(m_prog);
			m_prog.clearVersionInVector();
		}
		prog = prog.nextSibling();
	}
}
