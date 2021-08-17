#include "loading.h"
#include "ui_loading.h"
#include <QStringListModel>

Loading::Loading(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Loading)
{
	ui->setupUi(this);
	setWindowModality(Qt::ApplicationModal);
	connect(ui->btn_accept,
			SIGNAL(clicked(bool)),
			SLOT(slotAccept()));
	model = new QStringListModel;
	ui->listView->setModel(model);
	ui->btn_accept->setEnabled(false);
	ui->progressBar->reset();
}

Loading::~Loading()
{
	delete ui;
	delete model;
}

void Loading::setSize(int t_size)
{
	sizeOperation = t_size;
	ui->progressBar->setRange(0, sizeOperation);
}

void Loading::setValue()
{
	i++;
	ui->progressBar->setValue(i);
	if(i == sizeOperation)
		ui->btn_accept->setEnabled(true);
}

void Loading::slotAccept()
{
	ui->btn_accept->setEnabled(false);
	ui->progressBar->setValue(0);
	QDialog::accept();
	model->removeRows(0, sizeOperation);
	i=0;
}

void Loading::slotMkdir(bool ok, QString path)
{
	model->insertRow(model->rowCount());
	QModelIndex index = model->index(model->rowCount() - 1);
	QString msg = "Директория [" + path + (ok ? "] создана успешно" : "] не создана");
	model->setData(index, msg);
	setValue();
}

void Loading::slotRmdir(bool ok, QString path)
{
	model->insertRow(model->rowCount());
	QModelIndex index = model->index(model->rowCount() - 1);
	QString msg = "Директория [" + path + (ok ? "] удалена успешно" : "] не удалена");
	model->setData(index, msg);
	setValue();
}

void Loading::slotRemove(bool ok, QString path)
{
	model->insertRow(model->rowCount());
	QModelIndex index = model->index(model->rowCount() - 1);
	QString msg = "Файл [" + path + (ok ? "] удалён успешно" : "] не удалён");
	model->setData(index, msg);
}

void Loading::slotUpload(bool ok, QString localPath)
{
	model->insertRow(model->rowCount());
	QModelIndex index = model->index(model->rowCount() - 1);
	QString msg = "Файл [" + localPath + (ok ? "] загружен успешно" : "] не загружен");
	model->setData(index, msg);
	setValue();
}

void Loading::slotDownload(bool ok, QString remotePath)
{
	model->insertRow(model->rowCount());
	QModelIndex index = model->index(model->rowCount() - 1);
	QString msg = "Файл [" + remotePath + (ok ? "] скачан успешно" : "] не скачан");
	model->setData(index, msg);
	setValue();
}
