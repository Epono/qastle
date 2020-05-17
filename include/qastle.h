#pragma once

#include <QMainWindow>
#include <QHeaderView>
#include <QUndoStack>
#include "ui_qastle.h"
#include "ui_column.h"

#include "../include/table_model.h"
#include "../include/table_view.h"

class Qastle : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow ui;
	TableView* m_currentTableView;
	QVector<TableView*> m_tableViews;

	QWidget* m_plusTab;

public:
	Qastle(QWidget* parent = Q_NULLPTR);

	//QTableView* currentTableView();
	//void setCurrentTableView(QTableView* currentTableView);

public slots:
	void slotLoadFromJson();
	void slotSaveToJson();

	void addTab(const int selectedTabIndex = 0, TableModel* tableModel = new TableModel());
	void tabSelected(const int selectedTabIndex);
	void tabDoubleClicked(const int selectedTabIndex);
};
