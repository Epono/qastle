#pragma once

#include <QMainWindow>
#include <QTableView>
#include <QHeaderView>
#include "ui_qastle.h"
#include "ui_column.h"

#include "../include/table_model.h"

class Qastle : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow ui;
	QTableView* mCurrentTableView;
	QVector<QTableView*> mTableViews;

	QWidget* mPlusTab;

public:
	Qastle(QWidget* parent = Q_NULLPTR);
	void openModalAddColumn(const int selectedColumnIndex);

	//QTableView* currentTableView();
	//void setCurrentTableView(QTableView* currentTableView);

public slots:
	void slotLoadFromJson();
	void slotSaveToJson();

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
	void sideHeaderDoubleClicked(int logicalIndex);
	void showContextMenuGrid(const QPoint& pos);

	void addTab(const int selectedTabIndex = 0, TableModel* tableModel = new TableModel());
	void tabSelected(const int selectedTabIndex);
	void tabDoubleClicked(const int selectedTabIndex);
};
