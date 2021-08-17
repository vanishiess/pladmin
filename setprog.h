#ifndef SETPROG_H
#define SETPROG_H

#include <QDialog>
#include "prog.h"
#include "moveprog.h"

namespace Ui {
class SetProg;
}

class SetProg : public QDialog
{
	Q_OBJECT

public:
	explicit SetProg(QWidget *parent = 0);
	~SetProg();
	void xmlEditProg(QVector<Prog> *m_mas_prog, int t_row,
					 QVector<MoveProg> *m_moveProg, QVector<QStringList> *m_bufRemove);
	void xmlAddProg(QVector<Prog> *m_mas_prog, QVector<MoveProg> *m_moveProg);
	void setIdReadOnly(bool ok);
	void setAll(Prog t_prog);
	void setFlags();

private slots:
	void slotSetIcon();
	void slotSetDesc();
	void slotExam();

private:
	bool flagDesc, flagIcon;
	Ui::SetProg *ui;
};

#endif // SETPROG_H
