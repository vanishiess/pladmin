#include "versionlist.h"
#include "ui_versionlist.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include "xml.h"


VersionList::VersionList(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VersionList)
{
	m_mas_prog = new QVector<Prog>;
	m_MoveProg = new QVector<MoveProg>;
	m_bufRemove = new QVector<QStringList>;
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	connect(ui->btn_rem_ver,
				SIGNAL(clicked(bool)),
				SLOT(slotXmlButton_rem_ver()));
	connect(ui->btn_replace,
				SIGNAL(clicked(bool)),
				SLOT(slotXmlButton_replace()));
	connect(ui->btn_add_ver,
				SIGNAL(clicked(bool)),
				SLOT(slotXmlButton_add_ver()));
	connect(ui->tableView,
				SIGNAL(doubleClicked(QModelIndex)),
				SLOT(slotXmlButton_replace()));
}

VersionList::~VersionList()
{
	delete ui;
	delete m_xml;
	delete m_mas_prog;
	delete m_version;
	delete m_MoveProg;
	delete m_bufRemove;
}

void VersionList::slotXmlButton_add_ver()
{
	m_SetVersion = new setVersion;
	if(m_SetVersion->exec()== QDialog::Accepted){
		m_SetVersion->xml_add_ver(*m_mas_prog, this->GetRow(), *m_MoveProg);
		Prog m_prog = (*m_mas_prog)[this->GetRow()];
		this->GetList(m_prog);
	}
}

 void VersionList::slotXmlButton_rem_ver()
{
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){
		int row = selectModel -> selectedRows().first().row();

		MoveProg m_mProg;
		m_mProg.move = 3;
		m_mProg.appid = (*m_mas_prog)[this->GetRow()].getAppid();
		m_versions= (*m_mas_prog)[this->GetRow()].getVersions();
		m_mProg.id = m_versions[row].getId();
		m_MoveProg->push_back(m_mProg);

		(*m_mas_prog)[this->GetRow()].removeVersion(row);		//удаление происходит прямо тут
		Prog m_prog = (*m_mas_prog)[this->GetRow()];
		this->GetList(m_prog);
	}
}

void VersionList::slotXmlButton_replace()
{
	QItemSelectionModel *selectModel = ui->tableView->selectionModel();
	if (selectModel -> hasSelection()){
		int row = selectModel -> selectedRows().first().row();
		m_SetVersion = new setVersion;
		m_SetVersion->setIdReadOnly(true);
		QVector<Version> m_versions = ((*m_mas_prog)[this->GetRow()].getVersions());
		m_SetVersion->setAll(m_versions[row]);
		if(m_SetVersion->exec()== QDialog::Accepted){
			m_SetVersion->xml_replace(*m_mas_prog, this->GetRow(), *m_MoveProg, row, *m_bufRemove);
			Prog m_prog = (*m_mas_prog)[this->GetRow()];
			this->GetList(m_prog);
		}
	}
}

void VersionList::GetList(Prog t_prog)
{
	setWindowTitle(QString("Список версий " + t_prog.getName()));
	QStandardItemModel *model = new QStandardItemModel;
	QStandardItem *item;
	QStringList horizontalHeader;
	horizontalHeader.append("Id версии");
	horizontalHeader.append("Тип");
	horizontalHeader.append("Дата");
	horizontalHeader.append("Файл для запуска");

	model -> setHorizontalHeaderLabels(horizontalHeader);
	m_versions=t_prog.getVersions();
	for(int i=0; i<m_versions.size(); i++){
		item = new QStandardItem(m_versions[i].getId());
		model -> setItem(i, 0, item);
		switch (m_versions[i].getType().toInt()) {
		case 0:
			item = new QStandardItem("Неизвестная");
			break;
		case 1:
			item = new QStandardItem("Устаревшая");
			break;
		case 2:
			item = new QStandardItem("Не использовалась");
			break;
		case 3:
			item = new QStandardItem("Стабильная");
			break;
		case 4:
			item = new QStandardItem("Нестабильная");
			break;
		case 5:
			item = new QStandardItem("Выпуск");
			break;
		}
		model -> setItem(i, 1, item);
		item = new QStandardItem(m_versions[i].getDate());
		model -> setItem(i, 2, item);
		item = new QStandardItem(m_versions[i].getMainfile());
		model -> setItem(i, 3, item);
	}
	ui -> tableView -> setModel(model);
	ui -> tableView -> resizeColumnsToContents();
	ui -> tableView -> resizeRowsToContents();
}

int VersionList :: GetRow()
{
	return m_row;
}

void VersionList :: SetRow(int row)
{
	m_row=row;
}

void VersionList :: SetQVectorProg(QVector<Prog> t_mas_prog)
{
	*m_mas_prog = t_mas_prog;
}

void VersionList :: PushQVectorProg(QVector<Prog> &t_mas_prog)
{
	t_mas_prog = *m_mas_prog;
}

void VersionList :: SetQVectorMoveProg(QVector<MoveProg> t_MoveProg)
{
	*m_MoveProg=t_MoveProg;
}

void VersionList :: PushQVectorMoveProg(QVector<MoveProg> &t_mMoveProg)
{
	t_mMoveProg = *m_MoveProg;
}

void VersionList :: SetBufferRemove(QVector<QStringList> t_bufRemove)
{
	*m_bufRemove = t_bufRemove;
}

void VersionList :: PushBufferRemove(QVector<QStringList> &t_bufRemove)
{
	t_bufRemove = *m_bufRemove;
}
