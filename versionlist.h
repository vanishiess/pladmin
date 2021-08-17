#ifndef VERSIONLIST_H
#define VERSIONLIST_H

#include <QDialog>
#include <prog.h>
#include <setversion.h>
#include "moveprog.h"


namespace Ui {
class VersionList;
}

class VersionList : public QDialog
{
	Q_OBJECT

public:
	explicit VersionList(QWidget *parent = 0);
	~VersionList();
	void GetList(Prog m_prog);
	int GetRow();
	void SetRow(int row);
	void GetQVectorMoveProg(QVector<MoveProg> t_MoveProg);
	void SetQVectorMoveProg(QVector<MoveProg> t_MoveProg);
	void SetQVectorProg(QVector<Prog> t_mas_prog);
	void SetBufferRemove(QVector<QStringList> t_bufRemove);
	void PushQVectorProg(QVector<Prog> &t_mas_prog);
	void PushQVectorMoveProg(QVector<MoveProg> &t_mMoveProg);
	void PushBufferRemove(QVector<QStringList> &t_bufRemove);

private slots:
	void slotXmlButton_rem_ver();
	void slotXmlButton_add_ver();
	void slotXmlButton_replace();

private:
	int m_row;
	QString m_appid;
	Ui::VersionList *ui;
	XML *m_xml;
	Version *m_version;
	QVector<Prog> *m_mas_prog;
	QVector<MoveProg> *m_MoveProg;
	QVector<QStringList> *m_bufRemove;
	setVersion *m_SetVersion;
	QVector<Version> m_versions;
};

#endif // VERSIONLIST_H
