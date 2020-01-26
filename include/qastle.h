#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_qastle.h"
#include "ui_column.h"

class Qastle : public QMainWindow
{
	Q_OBJECT

public:
	Qastle(QWidget* parent = Q_NULLPTR);
	void openModalAddColumn(const int selectedColumnIndex);

public slots:
	void slotOpenModalAddColumnEnd();
	void slotColumnDialogAccepted(const int selectedColumnIndex, const QVariant data, const QString headerName);

	void slotRemoveColumn();
	void slotPrependColumn();
	void slotAppendColumn();

	void slotRemoveRow();
	void slotPrependRow();
	void slotAppendRow();

	void showContextMenuTopHeader(const QPoint& pos);
	void showContextMenuSideHeader(const QPoint& pos);
	void topHeaderDoubleClicked(int logicalIndex);
	void showContextMenuGrid(const QPoint& pos);
	void tabSelected(const int selectedTabIndex);
	void tabDoubleClicked(const int selectedTabIndex);

	

private:
	Ui::MainWindow ui;
};
