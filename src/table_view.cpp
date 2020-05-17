#include "../include/table_view.h"

#include <QClipboard>
#include <QApplication>
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QInputDialog>
#include <QDebug>

// TODO: no need for make_unique?
TableView::TableView(TableModel* tableModel, QWidget* parent) : QTableView(parent), m_tableModel(tableModel) {
	setModel(m_tableModel.get());

	setContextMenuPolicy(Qt::CustomContextMenu);
	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	verticalHeader()->setMinimumWidth(20);

	connect(this, &QWidget::customContextMenuRequested, this, &TableView::showContextMenuGrid);

	// HEADERS
	connect(horizontalHeader(), &QWidget::customContextMenuRequested, this, &TableView::showContextMenuTopHeader);
	connect(horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &TableView::topHeaderDoubleClicked);

	connect(verticalHeader(), &QWidget::customContextMenuRequested, this, &TableView::showContextMenuSideHeader);
	connect(verticalHeader(), &QHeaderView::sectionDoubleClicked, this, &TableView::sideHeaderDoubleClicked);
	//
}

TableView::~TableView() = default;

///////////////////////////////////////////////////////////////////////////////////////////////////
// SLOTS
void TableView::keyPressEvent(QKeyEvent * event)
{
	QItemSelectionModel* select = selectionModel();

	if (event->matches(QKeySequence::Delete))
	{
		// TODO: handle multiple selection
		if (select->hasSelection()) {
			clearCell(select->currentIndex());
		}
	}
	else if (event->matches(QKeySequence::Copy))
	{
		// TODO: handle multiple selection
		if (select->hasSelection()) {
			copyCell(select->currentIndex());
		}
	}
	else if (event->matches(QKeySequence::Paste))
	{
		// TODO: handle multiple selection
		if (select->hasSelection()) {
			pasteCell(select->currentIndex());
		}
	}
	else if (event->matches(QKeySequence::Undo)) {
		undo();
	}
	else if (event->matches(QKeySequence::Redo)) {
		redo();
	}
	else
	{
		QTableView::keyPressEvent(event);
	}
}

void TableView::clearCell(const QModelIndex & index) {
	m_tableModel->setData(index, QVariant(), Qt::EditRole);
}

void TableView::copyCell(const QModelIndex & index) {
	// TODO: toString() enough?
	QApplication::clipboard()->setText(m_tableModel->data(index).toString());
}

void TableView::pasteCell(const QModelIndex & index) {
	QastleType type = m_tableModel->dataModel()[index.column()];

	switch (type)
	{
	case QastleType::TString:
		m_tableModel->setData(index, QVariant(QApplication::clipboard()->text()).toString());
		break;
	case QastleType::TBool:
		m_tableModel->setData(index, QVariant(QApplication::clipboard()->text()).toBool());
		break;
	case QastleType::TInt:
		m_tableModel->setData(index, QVariant(QApplication::clipboard()->text()).toInt());
		break;
	case QastleType::TFloat:
		m_tableModel->setData(index, QVariant(QApplication::clipboard()->text()).toFloat());
		break;
	default:
		// TODO: 
		m_tableModel->setData(index, QVariant(QApplication::clipboard()->text()).toString());
		//qDebug() << QString("Unknown type : %1").arg(int(type));
		break;
	}
}

void TableView::undo() {
	m_tableModel->undoStack()->undo();
}
void TableView::redo() {
	m_tableModel->undoStack()->redo();
}

///////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// GRID
void TableView::showContextMenuGrid(const QPoint & pos) {
	QModelIndex index = indexAt(pos);

	m_menu = std::make_unique<QMenu>(this);

	// TODO: voir si ça peut pas déborder par la droite
	if (index.isValid()) {
		// Sur une des lignes
		m_menu->addAction(new QAction(QString("Col %1").arg(index.column()), this));
		m_menu->addAction(new QAction(QString("Row %1").arg(index.row()), this));
	}
	else {
		// En dehors des lignes
		m_menu->addAction(new QAction("Insert line", this));
	}

	m_menu->popup(viewport()->mapToGlobal(pos));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// HORIZONTAL HEADER

void TableView::showContextMenuTopHeader(const QPoint & pos) {
	QModelIndex index = indexAt(pos);

	m_menu = std::make_unique<QMenu>(this);

	QAction* actionAppendColumn;

	if (index.isValid()) {
		QAction* actionRemoveColumn = new QAction(QString("Remove column %1").arg(index.column()), this);
		actionRemoveColumn->setData(index);
		connect(actionRemoveColumn, &QAction::triggered, this, &TableView::slotRemoveColumn);
		m_menu->addAction(actionRemoveColumn);

		QAction* actionPrependColumn = new QAction(QString("Insert before column %1").arg(index.column()), this);
		actionPrependColumn->setData(index);
		connect(actionPrependColumn, &QAction::triggered, this, &TableView::slotPrependColumn);
		m_menu->addAction(actionPrependColumn);

		actionAppendColumn = new QAction(QString("Insert after column %1").arg(index.column()), this);
	}
	else {
		actionAppendColumn = new QAction(QString("Insert column"));
	}

	actionAppendColumn->setData(index);
	connect(actionAppendColumn, &QAction::triggered, this, &TableView::slotAppendColumn);
	m_menu->addAction(actionAppendColumn);

	m_menu->popup(viewport()->mapToGlobal(pos));
}

void TableView::slotRemoveColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	// TODO: ConfirmDialog
	m_tableModel->removeColumn(index.column(), index);
}

void TableView::slotPrependColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	openModalAddColumn(index.column());
}

void TableView::slotAppendColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	openModalAddColumn(index.column() + 1);
}

// https://stackoverflow.com/questions/13116863/qt-show-modal-dialog-ui-on-menu-item-click
void TableView::openModalAddColumn(const int selectedColumnIndex = -1) {
	m_columnDialog = std::make_unique<ColumnDialog>(this, selectedColumnIndex);

	connect(m_columnDialog.get(), &ColumnDialog::finishedWithData, this, &TableView::slotColumnDialogAccepted);
	m_columnDialog->show();
}

void TableView::slotColumnDialogAccepted(const int selectedColumnIndex, const QVariant data, const QString headerName) {
	QastleType type = static_cast<QastleType>(data.toInt());
	m_tableModel->insertColumnTyped(selectedColumnIndex, type, headerName);
}

void TableView::topHeaderDoubleClicked(int logicalIndex) {
	bool ok;

	if (logicalIndex == -1) {
		// Outside, new column
		openModalAddColumn(m_tableModel->headers().size());
	}
	else {
		QString text = QInputDialog::getText(this, QString("Change column name"), QString("Column name"), QLineEdit::Normal, m_tableModel->headers()[logicalIndex], &ok);
		if (ok && !text.isEmpty()) {
			m_tableModel->setHeaderAtIndex(logicalIndex, text);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// VERTICAL HEADER
void TableView::showContextMenuSideHeader(const QPoint & pos) {
	QModelIndex index = indexAt(pos);

	m_menu = std::make_unique<QMenu>(this);

	QAction* actionAppendRow;

	if (index.isValid()) {
		QAction* actionRemoveRow = new QAction(QString("Remove row %1").arg(index.row()), this);
		actionRemoveRow->setData(index);
		connect(actionRemoveRow, &QAction::triggered, this, &TableView::slotRemoveRow);
		m_menu->addAction(actionRemoveRow);

		QAction* actionPrependRow = new QAction(QString("Insert before row %1").arg(index.row()), this);
		actionPrependRow->setData(index);
		connect(actionPrependRow, &QAction::triggered, this, &TableView::slotPrependRow);
		m_menu->addAction(actionPrependRow);

		actionAppendRow = new QAction(QString("Insert after row %1").arg(index.row()), this);
	}
	else {
		actionAppendRow = new QAction(QString("Insert row"));
	}

	actionAppendRow->setData(index);
	connect(actionAppendRow, &QAction::triggered, this, &TableView::slotAppendRow);
	m_menu->addAction(actionAppendRow);

	m_menu->popup(viewport()->mapToGlobal(pos));
}

// https://stackoverflow.com/questions/28646914/qaction-with-custom-parameter
void TableView::slotRemoveRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	m_tableModel->removeRow(index.row(), index);
}

void TableView::slotPrependRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	m_tableModel->insertRow(index.row(), index);
}

void TableView::slotAppendRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	m_tableModel->insertRow(index.row() + 1, index);
}

void TableView::sideHeaderDoubleClicked(int logicalIndex) {
	if (logicalIndex == -1) {
		// Outside, new row
		m_tableModel->insertRow(m_tableModel->rowCount());
		//openModalAddColumn(model->headers().size());
	}
	else {
		// TODO: handle double click on row number
	}
}

/////////////////////////////////////////////////////////////////////////////////::
// Accessors

TableModel* TableView::tableModel() const {
	return m_tableModel.get();
}