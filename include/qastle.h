#pragma once

#include <QMainWindow>
#include <QHeaderView>
#include <QUndoStack>

#include <vector>

#include "ui_qastle.h"
#include "ui_column.h"

#include "../include/table_model.h"
#include "../include/table_view.h"

class Qastle : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow ui;

	std::vector<std::unique_ptr<TableView>> m_tableViews;

	std::unique_ptr<QWidget> m_plusTab;
	std::unique_ptr<QMenu> m_menu;

public:
	Qastle(QWidget* parent = Q_NULLPTR);
	Qastle(QString fileName, QWidget* parent = Q_NULLPTR);


	bool loadFromJson(const QString& fileName);
	bool saveToJson(const QString& fileName);

public slots:
	void slotLoadFromJson();
	void slotSaveToJson();

	void addTab(const int selectedTabIndex = 0, TableModel* tableModel = new TableModel());
	void slotTabSelected(const int selectedTabIndex);
	void slotTabDoubleClicked(const int selectedTabIndex);
	void showContextMenuTab(const QPoint& pos);

	void slotDeleteSheet();
};
