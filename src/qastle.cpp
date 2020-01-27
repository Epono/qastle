#include "qastle.h"
#include "table_model.h"
#include "import_export.h"

#include "column_dialog.h"

#include <QMenu>
#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>

Qastle::Qastle(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	ui.statusbar->showMessage(QString("This is the status bar"));

	TableModel* model = new TableModel();
	//model->addDummyData();
	ui.tableWidget->setModel(model);

	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->verticalHeader()->setMinimumWidth(20);

	(void)connect(ui.tableWidget, &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuGrid);

	// TABS
	(void)connect(ui.tabWidget, &QTabWidget::currentChanged, this, &Qastle::tabSelected);
	(void)connect(ui.tabWidget, &QTabWidget::tabBarDoubleClicked, this, &Qastle::tabDoubleClicked);

	// HEADERS
	(void)connect(ui.tableWidget->horizontalHeader(), &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuTopHeader);
	(void)connect(ui.tableWidget->horizontalHeader(), &QHeaderView::sectionDoubleClicked, this, &Qastle::topHeaderDoubleClicked);

	(void)connect(ui.tableWidget->verticalHeader(), &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuSideHeader);

	// ACTIONS
	(void)connect(ui.actionLoadFromJson, &QAction::triggered, this, &Qastle::slotLoadFromJson);
	(void)connect(ui.actionSaveToJson, &QAction::triggered, this, &Qastle::slotSaveToJson);

	(void)connect(ui.actionAddColumn, &QAction::triggered, this, &Qastle::slotOpenModalAddColumnEnd);
	(void)connect(ui.actionQuit, &QAction::triggered, this, &QApplication::quit);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD / SAVE
void Qastle::slotLoadFromJson() {
	QString fileName = QFileDialog::getOpenFileName(this,
		QString("Open Image"), QString(), QString("JSON file (*.json);; Binary file (*.dat)"));

	// TODO: make sur the file exists
	TableModel* model = ImportExport::loadFromJson(fileName);
	ui.tableWidget->setModel(model);
}

void Qastle::slotSaveToJson() {
	// TODO: check if a filepath is know or if it's a new one (save as vs save)
	const TableModel* model = qobject_cast<const TableModel*>(ui.tableWidget->model());

	QVector<QString> tabNames;
	int numberOfTabs = ui.tabWidget->count();
	for (int i = 0; i < numberOfTabs; ++i) {
		tabNames.append(ui.tabWidget->tabText(i));
	}
	bool result = ImportExport::saveToJson(QStringLiteral("data.json"), model, tabNames, ui.actionCheck->isChecked());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TABS
void Qastle::tabDoubleClicked(const int selectedTabIndex) {
	bool ok;
	QString text = QInputDialog::getText(this, QString("Change name"), QString("Sheet name"), QLineEdit::Normal, ui.tabWidget->tabText(selectedTabIndex), &ok);
	if (ok && !text.isEmpty()) {
		ui.tabWidget->setTabText(selectedTabIndex, text);
	}
}

//https://stackoverflow.com/questions/8308588/how-to-identify-when-the-current-tab-is-changing-in-a-qtabwidget
void Qastle::tabSelected(const int selectedTabIndex) {
	QString text(ui.tabWidget->tabText(selectedTabIndex));
	if (text.compare(QString("+")) == 0) {
		QWidget* newTab = new QWidget(ui.tabWidget);
		ui.tabWidget->addTab(newTab, tr("name"));

		ui.tabWidget->tabBar()->setCurrentIndex(ui.tabWidget->tabBar()->count() - 1);
		ui.tabWidget->tabBar()->moveTab(selectedTabIndex, ui.tabWidget->tabBar()->count() - 1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// GRID
void Qastle::showContextMenuGrid(const QPoint& pos) {
	QModelIndex index = ui.tableWidget->indexAt(pos);

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

	menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// HORIZONTAL HEADER

void Qastle::showContextMenuTopHeader(const QPoint& pos) {
	QModelIndex index = ui.tableWidget->indexAt(pos);

	QMenu* menu = new QMenu(this);

	if (index.isValid()) {
		QAction* actionRemoveColumn = new QAction(QString("Remove column %1").arg(index.column()), this);
		actionRemoveColumn->setData(index);
		(void)connect(actionRemoveColumn, &QAction::triggered, this, &Qastle::slotRemoveColumn);
		menu->addAction(actionRemoveColumn);

		QAction* actionPrependColumn = new QAction(QString("Insert before column %1").arg(index.column()), this);
		actionPrependColumn->setData(index);
		(void)connect(actionPrependColumn, &QAction::triggered, this, &Qastle::slotPrependColumn);
		menu->addAction(actionPrependColumn);
	}

	QAction* actionAppendColumn = new QAction(QString("Insert after column %1").arg(index.column()), this);
	actionAppendColumn->setData(index);
	(void)connect(actionAppendColumn, &QAction::triggered, this, &Qastle::slotAppendColumn);
	menu->addAction(actionAppendColumn);

	menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

void Qastle::slotRemoveColumn() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	// TODO: ConfirmDialog
	ui.tableWidget->model()->removeColumn(index.column(), index);
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
	const TableModel* model = qobject_cast<const TableModel*>(ui.tableWidget->model());
	openModalAddColumn(model->headers().size());
}

// https://stackoverflow.com/questions/13116863/qt-show-modal-dialog-ui-on-menu-item-click
void Qastle::openModalAddColumn(const int selectedColumnIndex = -1) {
	auto dialog = new ColumnDialog(this, selectedColumnIndex);

	(void)connect(dialog, &ColumnDialog::finishedWithData, this, &Qastle::slotColumnDialogAccepted);
	dialog->show();
}

void Qastle::slotColumnDialogAccepted(const int selectedColumnIndex, const QVariant data, const QString headerName) {
	const TableModel* model = qobject_cast<const TableModel*>(ui.tableWidget->model());
	TableModel* modelNonConst = const_cast<TableModel*>(model);

	QMetaType::Type type = QMetaType::Type(data.toInt());
	modelNonConst->insertColumnTyped(selectedColumnIndex, type, headerName);
}

void Qastle::topHeaderDoubleClicked(int logicalIndex) {
	bool ok;

	//on fait un premier cast pour convertir le QAbstractItemModel en Qfilesystemmodel
	const TableModel* model = qobject_cast<const TableModel*>(ui.tableWidget->model());

	QString text = QInputDialog::getText(this, QString("Change column name"), QString("Column name"), QLineEdit::Normal, model->headers()[logicalIndex], &ok);
	if (ok && !text.isEmpty()) {
		//on fait un second cast pour supprimer le const
		TableModel* modelNonConst = const_cast<TableModel*>(model);
		modelNonConst->headers()[logicalIndex] = text;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// VERTICAL HEADER
void Qastle::showContextMenuSideHeader(const QPoint& pos) {
	QModelIndex index = ui.tableWidget->indexAt(pos);

	QMenu* menu = new QMenu(this);

	if (index.isValid()) {
		QAction* actionRemoveRow = new QAction(QString("Remove row %1").arg(index.row()), this);
		actionRemoveRow->setData(index);
		(void)connect(actionRemoveRow, &QAction::triggered, this, &Qastle::slotRemoveRow);
		menu->addAction(actionRemoveRow);

		QAction* actionPrependRow = new QAction(QString("Insert before row %1").arg(index.row()), this);
		actionPrependRow->setData(index);
		(void)connect(actionPrependRow, &QAction::triggered, this, &Qastle::slotPrependRow);
		menu->addAction(actionPrependRow);
	}

	QAction* actionAppendRow = new QAction(QString("Insert after row %1").arg(index.row()), this);
	actionAppendRow->setData(index);
	(void)connect(actionAppendRow, &QAction::triggered, this, &Qastle::slotAppendRow);
	menu->addAction(actionAppendRow);

	menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

// https://stackoverflow.com/questions/28646914/qaction-with-custom-parameter
void Qastle::slotRemoveRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	ui.tableWidget->model()->removeRow(index.row(), index);
}

void Qastle::slotPrependRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	ui.tableWidget->model()->insertRow(index.row(), index);
}

void Qastle::slotAppendRow() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	ui.tableWidget->model()->insertRow(index.row() + 1, index);
}