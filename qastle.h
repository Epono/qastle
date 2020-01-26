#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qastle.h"

class Qastle : public QMainWindow
{
	Q_OBJECT

public:
	Qastle(QWidget *parent = Q_NULLPTR);

public slots:
	void slotRemoveColumn();
	void slotPrependColumn();
	void slotAppendColumn();

	void slotRemoveRow();
	void slotPrependRow();
	void slotAppendRow();

	void showContextMenuHorizontalHeader(const QPoint& pos);
	void showContextMenuVerticalHeader(const QPoint& pos);
	void horizontalHeaderDoubleClicked(int logicalIndex);
	void showContextMenuGrid(const QPoint& pos);
	void tabSelected(const int selectedTabIndex);
	void tabDoubleClicked(const int selectedTabIndex);
	void rien();

private:
	Ui::MainWindow ui;
};