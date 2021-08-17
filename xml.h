#ifndef XML_H
#define XML_H
#include "sftpclient.h"
#include "prog.h"
#include <QtXml>
#include <QFile>
#include <QString>


class XML
{
public:
	XML();
	void editXML(QVector<Prog> t_mas_prog);
	void readXML(QVector<Prog> &t_mas_prog);
private:
	SftpClient *m_sftpClient;

};

#endif // XML_H
