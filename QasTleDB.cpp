#include "QasTleDB.h"
#include "TableModel.h"
#include <QMenu>
#include <QDebug>
#include <QInputDialog>
#include <QDir>

QasTleDB::QasTleDB(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	auto model = new TableModel();

	ui.tableWidget->setModel(model);

	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->verticalHeader()->setMinimumWidth(20);

	connect(ui.tableWidget, &QWidget::customContextMenuRequested, this, &QasTleDB::showContextMenuGrid);

	ui.statusbar->showMessage(QString("looool"));

	connect(ui.actionRien, &QAction::triggered, this, &QasTleDB::rien);

	connect(ui.tabWidget, &QTabWidget::currentChanged, this, &QasTleDB::tabSelected);
	connect(ui.tabWidget, &QTabWidget::tabBarDoubleClicked, this, &QasTleDB::tabDoubleClicked);


	connect(ui.tableWidget->horizontalHeader(), &QWidget::customContextMenuRequested, this, &QasTleDB::showContextMenuHorizontalHeader);
	connect(ui.tableWidget->verticalHeader(), &QWidget::customContextMenuRequested, this, &QasTleDB::showContextMenuVerticalHeader);
}

void QasTleDB::tabDoubleClicked(const int selectedTabIndex) {
	bool ok;
	QString text = QInputDialog::getText(this, QString("Change name"), QString("Sheet name"), QLineEdit::Normal, ui.tabWidget->tabText(selectedTabIndex), &ok);
	if (ok && !text.isEmpty()) {
		qDebug() << text;
		ui.tabWidget->setTabText(selectedTabIndex, text);
	}
}

//https://stackoverflow.com/questions/8308588/how-to-identify-when-the-current-tab-is-changing-in-a-qtabwidget
void QasTleDB::tabSelected(const int selectedTabIndex) {
	QString text(ui.tabWidget->tabText(selectedTabIndex));
	if (text.compare(QString("+")) == 0) {
		QWidget* newTab = new QWidget(ui.tabWidget);
		ui.tabWidget->addTab(newTab, tr("name"));

		ui.tabWidget->tabBar()->setCurrentIndex(ui.tabWidget->tabBar()->count()-1);
		ui.tabWidget->tabBar()->moveTab(selectedTabIndex, ui.tabWidget->tabBar()->count() - 1);
	}
}

void QasTleDB::rien() {
	qDebug() << "lol";
}

void QasTleDB::showContextMenuGrid(const QPoint& pos) {
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

void QasTleDB::showContextMenuHorizontalHeader(const QPoint& pos) {
	QModelIndex index = ui.tableWidget->indexAt(pos);

	QMenu* menu = new QMenu(this);
	menu->addAction(new QAction("Action 1", this));
	menu->addAction(new QAction("Action 2", this));
	menu->addAction(new QAction("Action 3", this));
	menu->addAction(new QAction(QString("Column%1").arg(index.column()), this));
	menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

void QasTleDB::showContextMenuVerticalHeader(const QPoint& pos) {
	QModelIndex index = ui.tableWidget->indexAt(pos);

	QMenu* menu = new QMenu(this);
	
	if (index.isValid()) {
		QAction* actionRemoveLine = new QAction(QString("Remove line %1").arg(index.row()), this);
		actionRemoveLine->setData(index);
		connect(actionRemoveLine, &QAction::triggered, this, &QasTleDB::slotRemoveLine);
		menu->addAction(actionRemoveLine);
		
		QAction* actionPrependLine = new QAction(QString("Insert before line %1").arg(index.row()), this);
		actionPrependLine->setData(index);
		connect(actionPrependLine, &QAction::triggered, this, &QasTleDB::slotPrependLine);
		menu->addAction(actionPrependLine);
	}

	QAction* actionAppendLine = new QAction(QString("Insert after line %1").arg(index.row()), this);
	actionAppendLine->setData(index);
	connect(actionAppendLine, &QAction::triggered, this, &QasTleDB::slotAppendLine);
	menu->addAction(actionAppendLine);

	menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

// https://stackoverflow.com/questions/28646914/qaction-with-custom-parameter
void QasTleDB::slotRemoveLine() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	ui.tableWidget->model()->removeRow(index.row(), index);
}

void QasTleDB::slotPrependLine() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	ui.tableWidget->model()->insertRow(index.row() - 1, index);
}

void QasTleDB::slotAppendLine() {
	QModelIndex index = qobject_cast<QAction*>(sender())->data().toModelIndex();
	ui.tableWidget->model()->insertRow(index.row(), index);
}
