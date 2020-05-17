#include "../include/qastle.h"
#include "../include/import_export.h"

#include "../include/utils.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>

Qastle::Qastle(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	setWindowTitle(QApplication::applicationDisplayName());
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

	//(void)connect(ui.actionAddColumn, &QAction::triggered, this, &Qastle::slotOpenModalAddColumnEnd);
	(void)connect(ui.actionQuit, &QAction::triggered, this, &QApplication::quit);
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
		TableView* tableView = m_tableViews[i];
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
	for (TableView* tableView : m_tableViews) {
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
	TableView* tableView = new TableView(tableModel);
	layout->addWidget(tableView);

	// Init table view
	// TODO: add default name

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



