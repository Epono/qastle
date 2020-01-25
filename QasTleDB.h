#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QasTleDB.h"

class QasTleDB : public QMainWindow
{
	Q_OBJECT

public:
	QasTleDB(QWidget *parent = Q_NULLPTR);

public slots:
	void slotRemoveLine();
	void slotPrependLine();
	void slotAppendLine();

	void showContextMenuHorizontalHeader(const QPoint& pos);
	void showContextMenuVerticalHeader(const QPoint& pos);
	void showContextMenuGrid(const QPoint& pos);
	void tabSelected(const int selectedTabIndex);
	void tabDoubleClicked(const int selectedTabIndex);
	void rien();

private:
	Ui::MainWindow ui;
};
