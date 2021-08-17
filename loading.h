#ifndef LOADING_H
#define LOADING_H

#include <QDialog>
#include <QStringListModel>

namespace Ui {
class Loading;
}

class Loading : public QDialog
{
	Q_OBJECT

public:
	explicit Loading(QWidget *parent = 0);
	~Loading();
	void setSize(int t_size);
	void setValue();

private slots:
	void slotAccept();
	void slotMkdir(bool ok, QString path);
	void slotRmdir(bool ok, QString path);
	void slotRemove(bool ok, QString path);
	void slotUpload(bool ok, QString localPath);
	void slotDownload(bool ok, QString remotePath);

private:
	Ui::Loading *ui;
	int sizeOperation;
	int i = 0;				//счётчик для прогрессбара
	QStringListModel *model;
};

#endif // LOADING_H
