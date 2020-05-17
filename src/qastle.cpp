#include "../include/qastle.h"
#include "../include/import_export.h"

#include "../include/utils.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QDir>
#include <QListWidget>

Qastle::Qastle(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);

	setWindowTitle(QApplication::applicationDisplayName());
	ui.statusbar->showMessage(QString("This is the status bar"));

	m_plusTab = std::make_unique<QWidget>(ui.tabWidget->widget(0));

	// Add one (TODO: default) tab
	//auto defaultTableModel = std::make_unique<TableModel>();
	auto defaultTableModel = new TableModel();
	defaultTableModel->addFirstColumnAndRow();
	addTab(0, defaultTableModel);

	// TABS
	connect(ui.tabWidget, &QTabWidget::currentChanged, this, &Qastle::slotTabSelected);
	connect(ui.tabWidget, &QTabWidget::tabBarDoubleClicked, this, &Qastle::slotTabDoubleClicked);

	ui.tabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.tabWidget, &QWidget::customContextMenuRequested, this, &Qastle::showContextMenuTab);


	// ACTIONS
	connect(ui.actionLoadFromJson, &QAction::triggered, this, &Qastle::slotLoadFromJson);
	connect(ui.actionSaveToJson, &QAction::triggered, this, &Qastle::slotSaveToJson);

	connect(ui.actionQuit, &QAction::triggered, this, &QApplication::quit);
}

Qastle::Qastle(QString fileName, QWidget* parent) : Qastle(parent) {
	if (Utils::fileExists(fileName)) {
		loadFromJson(fileName);
	}
	else {
		qDebug() << "File not found, ignoring and loading blank data";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD / SAVE
void Qastle::slotLoadFromJson() {
	// TODO: "Do you want to save current session?"

	QString fileName = QFileDialog::getOpenFileName(this, QString("Load file"), QString(), QString("JSON file (*.json);; Binary file (*.dat)"));

	// Cancelled dialog
	if (fileName.isEmpty()) {
		return;
	}

	// Make sur the file exists
	if (!Utils::fileExists(fileName)) {
		return;
	}

	loadFromJson(fileName);
}

bool Qastle::loadFromJson(const QString& fileName) {
	ui.tabWidget->blockSignals(true);

	// Delete views
	m_tableViews.clear();

	// Remove and delete old tabs (except not deleting "+" tab)
	// TODO: Still useful with the way Qt manages memory?
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
		addTab(i, tableModels[i]);
	}

	ui.tabWidget->addTab(m_plusTab.get(), "+");
	ui.tabWidget->tabBar()->setCurrentIndex(0);

	ui.tabWidget->blockSignals(false);

	return true;
}

void Qastle::slotSaveToJson() {
	// TODO: check if a filepath is known or if it's a new one (save as vs save)

	QString fileName = QFileDialog::getSaveFileName(this, QString("Save file"), QString(), QString("JSON file (*.json)"));

	saveToJson(fileName);
}

bool Qastle::saveToJson(const QString& fileName) {
	QVector<TableModel*> tableModels;
	// https://stackoverflow.com/questions/8237502/iterating-over-a-container-of-unique-ptrs/27231261
	for (auto&& tableView : m_tableViews) {
		tableModels.append(tableView->tableModel());
	}
	bool result = ImportExport::saveToJson(fileName, tableModels, ui.actionCheck->isChecked());

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// TABS
void Qastle::slotTabDoubleClicked(const int selectedTabIndex) {
	if (ui.tabWidget->tabText(selectedTabIndex).compare(QString("+")) == 0) {
		// "+" tab double clicked, ignore
		return;
	}

	bool ok;
	QString text = QInputDialog::getText(this, QString("Change name"), QString("Sheet name"), QLineEdit::Normal, ui.tabWidget->tabText(selectedTabIndex), &ok);
	if (ok && !text.isEmpty()) {
		m_tableViews[selectedTabIndex]->tableModel()->setSheetName(text);
		ui.tabWidget->setTabText(selectedTabIndex, text);
	}
}

void Qastle::addTab(const int selectedTabIndex, TableModel* tableModel) {
	// TODO: handle sheets with "@" (meaning sub sheet)
	if (tableModel->sheetName().contains("@")) {
		return;
	}

	// Block signals to avoid infinite loop with "+" tab
	ui.tabWidget->blockSignals(true);

	QWidget* newTab = new QWidget(ui.tabWidget);
	QGridLayout* layout = new QGridLayout(newTab);
	auto tableView = std::make_unique<TableView>(tableModel, newTab);
	layout->addWidget(tableView.get());

	// Init table view
	// TODO: add default name

	m_tableViews.push_back(std::move(tableView));

	ui.tabWidget->insertTab(selectedTabIndex, newTab, tableModel->sheetName());
	//ui.tabWidget->setTabText(selectedTabIndex, tableModel->sheetName());
	ui.tabWidget->tabBar()->setCurrentIndex(selectedTabIndex);

	ui.tabWidget->blockSignals(false);
}

//https://stackoverflow.com/questions/8308588/how-to-identify-when-the-current-tab-is-changing-in-a-qtabwidget
void Qastle::slotTabSelected(const int selectedTabIndex) {
	// TODO: string compare or last place compare?
	if (ui.tabWidget->tabText(selectedTabIndex).compare(QString("+")) == 0) {
		TableModel* tableModel = new TableModel();
		tableModel->addFirstColumnAndRow();
		addTab(selectedTabIndex, tableModel);
	}
}


void Qastle::showContextMenuTab(const QPoint& pos) {
	int tabIndex = ui.tabWidget->tabBar()->tabAt(pos);
	if (tabIndex == -1 || tabIndex == ui.tabWidget->count()) {
		// Outside or "+", ignore
		return;
	}
	QPoint globalPos = mapToGlobal(pos);

	m_menu = std::make_unique<QMenu>(this);

	QAction* actionAppendColumn;
	actionAppendColumn = new QAction(QString("Delete sheet"));
	actionAppendColumn->setData(tabIndex);
	connect(actionAppendColumn, &QAction::triggered, this, &Qastle::slotDeleteSheet);
	m_menu->addAction(actionAppendColumn);

	m_menu->popup(globalPos);
}

void Qastle::slotDeleteSheet() {
	// TODO: handle when only one real sheet
	int tabIndex = qobject_cast<QAction*>(sender())->data().toInt();
	if (ui.tabWidget->currentIndex() == tabIndex) {
		ui.tabWidget->tabBar()->setCurrentIndex(tabIndex - 1);
	}

	m_tableViews.erase(m_tableViews.begin() + tabIndex);
	ui.tabWidget->removeTab(tabIndex);
}