#pragma once

#include <QtWidgets/QWidget>
#include <QAbstractTableModel>
#include <vector>

class TableModel : public QAbstractTableModel {

	Q_OBJECT

private:
	// TODO: en faire un type et avoir un vector de ce type ?
	QVector <QMetaType::Type> dataModel;

	QVector <QVariant> newLineTemplate;

	QVector <QVector <QVariant> > tableData;

	void TableModel::addTypeToTemplate(int column, const QMetaType::Type type, const QString headerName);
	void TableModel::addTypeToExistingData(int column, const QMetaType::Type type);

public:
	// TODO: accessor
	QVector <QString> headers;

	TableModel(QObject* parent = nullptr);

	Qt::ItemFlags TableModel::flags(const QModelIndex& index) const;

	int TableModel::rowCount(const QModelIndex& parent = QModelIndex()) const;
	int TableModel::columnCount(const QModelIndex& parent = QModelIndex()) const;

	bool TableModel::insertColumnTyped(int column, const QMetaType::Type type, const QString headerName, const QModelIndex& parent = QModelIndex());
	//bool TableModel::insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
	bool TableModel::removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());

	bool TableModel::insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool TableModel::removeRows(int row, int count, const QModelIndex& parent = QModelIndex());


	QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QVariant TableModel::data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};