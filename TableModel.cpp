#include "TableModel.h"

#include <QDebug>

TableModel::TableModel(QObject* parent) {
	for (int i = 0; i < 10; i++)
	{
		QVector<QVariant> tempVector;

		for (int j = 0; j < 10; j++)
		{
			tempVector.push_back((10*i)+j);
		}
		tableData.push_back(tempVector);
	}
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const {
	//auto currentFlags = this->flags(index);
	//return currentFlags & (~Qt::ItemIsUserCheckable);
	//return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	//return (Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable) & (~Qt::ItemIsUserCheckable);

	return (QAbstractItemModel::flags(index) | Qt::ItemIsEditable) & (~Qt::ItemIsUserCheckable);

}

int TableModel::rowCount(const QModelIndex& parent) const {
	return tableData.size();
}

int TableModel::columnCount(const QModelIndex& parent) const {
	if (tableData.size() > 0) {
		return tableData[0].size();
	}
	else {
		return 0;
	}
}

//https://www.qtcentre.org/threads/55406-how-i-can-chagne-Qtableview-horizontal-header-text-as-well-as-remove-Indexes-vHeader
QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (orientation == Qt::Orientation::Vertical) {
		if (role == Qt::ItemDataRole::DisplayRole) {
			return QString::number(section);
		}
		return QVariant();
	}
	else {
		if (role == Qt::ItemDataRole::DisplayRole) {
			return QString("Col%1").arg(section);
		}
		return QVariant();
	}
}

// https://stackoverflow.com/questions/55855284/qabstracttablemodel-editing-without-clearing-previous-data-in-cell
bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	int x = index.column();
	int y = index.row();
	
	if (role == Qt::EditRole) {
		if (!checkIndex(index)) {
			return false;
		}
		tableData[index.row()][index.column()] = value;
		return true;
	}

	return false;
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	if (role == Qt::ItemDataRole::DisplayRole) {
		return tableData[index.row()][index.column()];
	}
	else if (role == Qt::ItemDataRole::EditRole) {
		return tableData[index.row()][index.column()];
	}
	return QVariant();
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent) {
	emit(this->beginInsertRows(QModelIndex(), row, row + count - 1));

	for (int i = 0; i < count; ++i) {
		QVector<QVariant> newVector(tableData[0].size());
		tableData.insert(row + 1, newVector);
	}

	emit(this->endInsertRows());
	return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent) {
	emit(this->beginRemoveRows(QModelIndex(), row, row + count - 1));

	tableData.remove(row, count);

	emit(this->endRemoveRows());
	return true;
}