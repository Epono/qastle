#pragma once
#include <QTableView>

#include "../include/table_model.h"
#include "../include/column_dialog.h"

class TableView : public QTableView
{
	Q_OBJECT

private:
	std::unique_ptr<TableModel> m_tableModel;
	std::unique_ptr<ColumnDialog> m_columnDialog;
	std::unique_ptr<QMenu> m_menu;

public:
	TableView(TableModel* tableModel, QWidget* parent = Q_NULLPTR);
	~TableView();

	void clearCell(const QModelIndex& index);
	void copyCell(const QModelIndex& index);
	void pasteCell(const QModelIndex& index);

	void undo();
	void redo();

	void openModalAddColumn(const int selectedColumnIndex);

	TableModel* tableModel() const;

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