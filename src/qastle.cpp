#include "../include/qastle.h"
#include "../include/import_export.h"

#include "../include/column_dialog.h"

#include "../include/utils.h"

#include <QMenu>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>

Qastle::Qastle(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	ui.statusbar->showMessage(QString("This is the status bar"));

	m_plusTab = ui.tabWidget->widget(0);

	// Add one (TODO: default) tab
	TableModel* defaultTableModel = new TableModel();
	defaultTableModel->addFirstColumnAndRow();
	addTab(0, defaultTableModel);

	// TABS
	(void)connect(ui.tabWidget, &QTabWidget::currentChanged, this, &Qastle::tabSelected);
	(void)connect(ui.tabWidget, &QTabWidget::tabBarDoubleClicked, this, &Qastle::tabDoubleClicked);

	// ACTIONS
	(void)connect(ui.actionLoadFromJson, &QAction::triggered, this, &Qastle::slotLoadFromJson);
	(void)connect(ui.actionSaveToJson, &QAction::triggered, this, &Qastle::slotSaveToJson);

	(void)connect(ui.actionAddColumn, &QAction::triggered, this, &Qastle::slotOpenModalAddColumnEnd);
	(void)connect(ui.actionQuit, &QAction::triggered, this, &QApplication::quit);

	(void)connect(ui.actionRedo, &QAction::triggered, this, &Qastle::slotRedo);
	(void)connect(ui.actionUndo, &QAction::triggered, this, &Qastle::slotUndo);

	//undoAction = undoStack->createUndoAction(this, tr("&Undo"));
	//undoAction->setShortcuts(QKeySequence::Undo);

	//redoAction = undoStack->createRedoAction(this, tr("&Redo"));
	//redoAction->setShortcuts(QKeySequence::Redo);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// REDO / UNDO
void Qastle::slotRedo() {
	qDebug() << "Redo";

	const TableModel* model = qobject_cast<const TableModel*>(m_currentTableView->model());
	TableModel* modelNonConst = const_cast<TableModel*>(model);
	modelNonConst->undoStack()->redo();
}

void Qastle::slotUndo() {
	qDebug() << "Undo";

	const TableModel* model = qobject_cast<const TableModel*>(m_currentTableView->model());
	TableModel* modelNonConst = const_cast<TableModel*>(model);
	modelNonConst->undoStack()->undo();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD / SAVE
void Qastle::slotLoadFromJson() {
	// TODO: "Do you want to save current session?"

	QString fileName = QFileDialog::getOpenFileName(this, QString("Load file"), QString(), QString("JSON file (*.json);; Binary file (*.dat)"));
	
	// Dialog cancelled
	if (fileName.isEmpty()) {
		return;
	}

	// TODO: make sur the file exists

	ui.tabWidget->blockSignals(true);

	// Delete views
	for (int i = 0; i < m_tableViews.size(); ++i) {
		QTableView* tableView = m_tableViews[i];
		delete tableView;
	}
	m_tableViews.clear();

	// Remove and delete old tabs (except not deleting "+" tab)
	int temp = 0;
	while (ui.tabWidget->count() > 1) {
		QWidget* tab = ui.tabWidget->widget(temp);
		if (ui.tabWidget->tabText(temp).compare(QString("+")) == 0) {
			continue;
		}
		else {
			delete tab;
		}
	}
	ui.tabWidget->clear();
	//

	QVector<TableModel*> tableModels = ImportExport::loadFromJson(fileName);

	for (int i = 0; i < tableModels.size(); ++i) {
		TableModel* tableModel = tableModels[i];

		addTab(i, tableModel);
	}

	ui.tabWidget->addTab(m_plusTab, "+");

	ui.tabWidget->tabBar()->setCurrentIndex(0);
	m_currentTableView = m_tableViews[0];

	ui.tabWidget->blockSignals(false);
}

void Qastle::slotSaveToJson() {
	// TODO: check if a filepath is know or if it's a new one (save as vs save)

	QString fileName = QFileDialog::getSaveFileName(this, QString("Save file"), QString(), QString("JSON file (*.json)"));

	QVector<TableModel*> tableModels;
	for (QTableView* tableView : m_tableViews) {
		const TableModel* model = qobject_cast<const TableModel*>(tableView->model());
		TableModel* modelNonConst = const_cast<TableModel*>(model);
		tableModels.append(modelNonConst);
	}
	bool result = ImportExport::saveToJson(fileName, tableModels, ui.actionCheck->isChecked());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TABS
void Qastle::tabDoubleClicked(const int selectedTabIndex) {
	bool ok;
	QString text = QInputDialog::getText(this, QString("Change name"), QString("Sheet name"), QLineEdit::Normal, ui.tabWidget->tabText(selectedTabIndex), &ok);
	if (ok && !text.isEmpty()) {
		const TableModel* model = qobject_cast<const TableModel*>(m_tableViews[selectedTabIndex]->model());
		TableModel* modelNonConst = const_cast<TableModel*>(model);

		modelNonConst->setSheetName(text);
		ui.tabWidget->setTabText(selectedTabIndex, text);
	}
}

void Qastle::addTab(const int selectedTabIndex, TableModel* tableModel) {
	// TODO: handle sheets with "@" (meaning sub sheet)
	if (tableModel->sheetName().contains("@")) {
		return;
	}

	ui.tabWidget->blockSignals(true);

	QWidget* newTab = new QWidget(ui.tabWidget);
	QGridLayout* layout = new QGridLayout(newTab);
	QTableView* tableView = new QTableView();
	layout->addWidget(tableView);

	// Init table view
	// TODO: add default name

	tableView->setModel(tableModel);

	tableView->setContextMenuPolicy(Qt::CustomContextMenu);
	tableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	tableView->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	tableView->verticalHeader()->setMinimumWidth(20);

	(void)connect(tableView, &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuGrid);

	// HEADERS
	(void)connect(tableView->horizontalHeader(), &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuTopHeader);
	(void)connect(tableView->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &Qastle::topHeaderDoubleClicked);

	(void)connect(tableView->verticalHeader(), &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuSideHeader);
	(void)connect(tableView->verticalHeader(), &QHeaderView::sectionDoubleClicked, this, &Qastle::sideHeaderDoubleClicked);
	//

	m_tableViews.append(tableView);
	m_currentTableView = tableView;

	ui.tabWidget->insertTab(selectedTabIndex, newTab, tableModel->sheetName());
	ui.tabWidget->setTabText(selectedTabIndex, tableModel->sheetName());

	// Move "+" back at the end
	ui.tabWidget->tabBar()->setCurrentIndex(selectedTabIndex);

	ui.tabWidget->blockSignals(false);
}

//https://stackoverflow.com/questions/8308588/how-to-identify-when-the-current-tab-is-changing-in-a-qtabwidget
void Qastle::tabSelected(const int selectedTabIndex) {
	QString text(ui.tabWidget->tabText(selectedTabIndex));

	// TODO: string compare or last place compare?
	if (text.compare(QString("+")) == 0) {
		TableModel* tableModel = new TableModel();
		tableModel->addFirstColumnAndRow();
		addTab(selectedTabIndex, tableModel);
	}
	else {
		m_currentTableView = m_tableViews[selectedTabIndex];
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// GRID
void Qastle::showContextMenuGrid(const QPoint& pos) {
	QModelIndex index = m_currentTableView->indexAt(pos);

	QMenu* menu = new QMenu(this);
	// TODO: voir si ça peut pas déborder par la droite
	if (index.isValid()) {
		// Sur une des lignes
		menu->addAction(new QAction("Quelle jolie cellule", this));
		menu->addAction(new QAction(QString("Col %1").arg(index.column()), this));
		menu->addAction(new QAction(QString("Row %1").arg(index.row()), this));
	}
	else {
		// En dehors des lignes
		menu->addAction(new QAction("Quelle joli vide", this));
		menu->addAction(new QAction("Ajouter une ligne", this));
	}

	menu->popup(m_currentTableView->viewport()->mapToGlobal(pos));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// HORIZONTAL HEADER

void Qastle::showContextMenuTopHeader(const QPoint& pos) {
	QModelIndex index = m_currentTableView->indexAt(pos);

	QMenu* menu = new QMenu(this);

	QAction* actionAppendColumn;

	if (index.isValid()) {
		QAction* actionRemoveColumn = new QAction(QString("Remove column %1").arg(index.column()), this);
		actionRemoveColumn->setData(index);
		(void)connect(actionRemoveColumn, &QAction::triggered, this, &Qastle::slotRemoveColumn);
		menu->addAction(actionRemoveColumn);

		QAction* actionPrependColumn = new QAction(QString("Insert before column %1").arg(index.column()), this);
		actionPrependColumn->setData(index);
		(void)connect(actionPrependColumn, &QAction::triggered, this, &Qastle::slotPrependColumn);
		menu->addAction(actionPrependColumn);

		actionAppendColumn = new QAction(QString("Insert after column %1").arg(index.column()), this);
	}
	else {
		actionAppendColumn = new QAction(QString("Insert column"));
	}

	actionAppendColumn->setData(index);
	(void)connect(actionAppendColumn, &QAction::triggered, this, &Qastle::slotAppendColumn);
	menu->addAction(actionAppendColumn);

	menu->popup(m_currentTableView->viewport()->mapToGlobal(pos));
}

void Qastle::slotRemoveColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	// TODO: ConfirmDialog
	m_currentTableView->model()->removeColumn(index.column(), index);
}

void Qastle::slotPrependColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	openModalAddColumn(index.column());
}

void Qastle::slotAppendColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	openModalAddColumn(index.column() + 1);
}

void Qastle::slotOpenModalAddColumnEnd() {
	const TableModel* model = qobject_cast<const TableModel*>(m_currentTableView->model());
	openModalAddColumn(model->headers().size());
}

// https://stackoverflow.com/questions/13116863/qt-show-modal-dialog-ui-on-menu-item-click
void Qastle::openModalAddColumn(const int selectedColumnIndex = -1) {
	auto dialog = new ColumnDialog(this, selectedColumnIndex);

	(void)connect(dialog, &ColumnDialog::finishedWithData, this, &Qastle::slotColumnDialogAccepted);
	dialog->show();
}

void Qastle::slotColumnDialogAccepted(const int selectedColumnIndex, const QVariant data, const QString headerName) {
	const TableModel* model = qobject_cast<const TableModel*>(m_currentTableView->model());
	TableModel* modelNonConst = const_cast<TableModel*>(model);

	QastleType type = static_cast<QastleType>(data.toInt());
	modelNonConst->insertColumnTyped(selectedColumnIndex, type, headerName);
}

void Qastle::topHeaderDoubleClicked(int logicalIndex) {
	bool ok;

	//on fait un premier cast pour convertir le QAbstractItemModel en Qfilesystemmodel
	const TableModel* model = qobject_cast<const TableModel*>(m_currentTableView->model());
	if (logicalIndex == -1) {
		// Outside, new column
		openModalAddColumn(model->headers().size());
	}
	else {
		QString text = QInputDialog::getText(this, QString("Change column name"), QString("Column name"), QLineEdit::Normal, model->headers()[logicalIndex], &ok);
		if (ok && !text.isEmpty()) {
			//on fait un second cast pour supprimer le const
			TableModel* modelNonConst = const_cast<TableModel*>(model);
			modelNonConst->setHeaderAtIndex(logicalIndex, text);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// VERTICAL HEADER
void Qastle::showContextMenuSideHeader(const QPoint& pos) {
	QModelIndex index = m_currentTableView->indexAt(pos);

	QMenu* menu = new QMenu(this);

	QAction* actionAppendRow;

	if (index.isValid()) {
		QAction* actionRemoveRow = new QAction(QString("Remove row %1").arg(index.row()), this);
		actionRemoveRow->setData(index);
		(void)connect(actionRemoveRow, &QAction::triggered, this, &Qastle::slotRemoveRow);
		menu->addAction(actionRemoveRow);

		QAction* actionPrependRow = new QAction(QString("Insert before row %1").arg(index.row()), this);
		actionPrependRow->setData(index);
		(void)connect(actionPrependRow, &QAction::triggered, this, &Qastle::slotPrependRow);
		menu->addAction(actionPrependRow);

		actionAppendRow = new QAction(QString("Insert after row %1").arg(index.row()), this);
	}
	else {
		actionAppendRow = new QAction(QString("Insert row"));
	}
	
	actionAppendRow->setData(index);
	(void)connect(actionAppendRow, &QAction::triggered, this, &Qastle::slotAppendRow);
	menu->addAction(actionAppendRow);

	menu->popup(m_currentTableView->viewport()->mapToGlobal(pos));
}

// https://stackoverflow.com/questions/28646914/qaction-with-custom-parameter
void Qastle::slotRemoveRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	m_currentTableView->model()->removeRow(index.row(), index);
}

void Qastle::slotPrependRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	m_currentTableView->model()->insertRow(index.row(), index);
}

void Qastle::slotAppendRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	m_currentTableView->model()->insertRow(index.row() + 1, index);
}

void Qastle::sideHeaderDoubleClicked(int logicalIndex) {
	//on fait un premier cast pour convertir le QAbstractItemModel en Qfilesystemmodel
	const TableModel* model = qobject_cast<const TableModel*>(m_currentTableView->model());
	if (logicalIndex == -1) {
		// Outside, new row
		m_currentTableView->model()->insertRow(model->rowCount());
		//openModalAddColumn(model->headers().size());
	}
	else {
		// TODO: handle double click on row number
	}
}