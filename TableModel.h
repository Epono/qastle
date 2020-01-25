#pragma once

#include <QtWidgets/QWidget>
#include <QAbstractTableModel>
#include <vector>

class TableModel : public QAbstractTableModel {

private:
	QVector <QVector <QVariant> > tableData;
	QVector <QVariant> dataModel;

public:
	TableModel(QObject* parent = nullptr);

	Qt::ItemFlags TableModel::flags(const QModelIndex& index) const;

	int TableModel::rowCount(const QModelIndex& parent = QModelIndex()) const;
	int TableModel::columnCount(const QModelIndex& parent = QModelIndex()) const;

	bool TableModel::insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool TableModel::removeRows(int row, int count, const QModelIndex& parent = QModelIndex());


	QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QVariant TableModel::data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};