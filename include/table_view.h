#pragma once
#include <QTableView>

#include "../include/table_model.h"

class TableView : public QTableView
{
	Q_OBJECT

private:
	TableModel* m_tableModel;

public:
	TableView(TableModel* tableModel);

	void clearCell(const QModelIndex& index);
	void copyCell(const QModelIndex& index);
	void pasteCell(const QModelIndex& index);

	void undo();
	void redo();

	void openModalAddColumn(const int selectedColumnIndex);

public slots:
	void keyPressEvent(QKeyEvent* event);

	void showContextMenuGrid(const QPoint& pos);

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
};

