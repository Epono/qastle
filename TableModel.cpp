#include "TableModel.h"

#include <QDebug>

TableModel::TableModel(QObject* parent) {

	dataModel.push_back(QMetaType::Type::Bool);
	dataModel.push_back(QMetaType::Type::Int);
	dataModel.push_back(QMetaType::Type::Float);
	dataModel.push_back(QMetaType::Type::Double);
	dataModel.push_back(QMetaType::Type::QString);

	headers.push_back("Dead?");
	headers.push_back("Age");
	headers.push_back("Height");
	headers.push_back("Weight");
	headers.push_back("Name");

	for (int i = 0; i < dataModel.size(); ++i) {
		QMetaType::Type type = QMetaType::Type(dataModel[i]);
		if (type != QMetaType::Type::UnknownType) {
			//void* ptr = QMetaType::create(type);

			switch (type)
			{
			case QMetaType::QString:
				newLineTemplate.push_back(QString());
				break;
			case QMetaType::Bool:
				newLineTemplate.push_back(false);
				break;
			case QMetaType::Int:
				newLineTemplate.push_back(int(0));
				break;
			case QMetaType::Float:
				newLineTemplate.push_back(float(0));
				break;
			case QMetaType::Double:
				newLineTemplate.push_back(double(0));
				break;
			}

			//QMetaType::destroy(type, ptr);
			//ptr = 0;
		}
	}

	QVector<QVariant> tempVector(newLineTemplate);
	tableData.push_back(tempVector);
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const {
	return (QAbstractItemModel::flags(index) | Qt::ItemFlag::ItemIsEditable) & (~Qt::ItemFlag::ItemIsUserCheckable);
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
			return QString("%1\n(%2)").arg(headers[section]).arg(QMetaType::typeName(dataModel[section]));
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

bool TableModel::insertColumns(int column, int count, const QModelIndex& parent) {
	emit(this->beginInsertColumns(QModelIndex(), column, column + count - 1));

	for (int i = 0; i < count; ++i) {
		dataModel.insert(column, QMetaType::Type::QString);
		newLineTemplate.insert(column, QString());
		headers.insert(column, "New column");

		for (int j = 0; j < tableData.size(); ++j) {
			tableData[j].insert(column, QString());
		}
	}

	emit(this->endInsertColumns());
	return true;
}

bool TableModel::removeColumns(int column, int count, const QModelIndex& parent) {
	emit(this->beginRemoveColumns(QModelIndex(), column, column + count - 1));

	dataModel.remove(column, count);
	newLineTemplate.remove(column, count);
	headers.remove(column, count);

	for (int j = 0; j < tableData.size(); ++j) {
		tableData[j].remove(column, count);
	}

	emit(this->endRemoveColumns());
	return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent) {
	emit(this->beginInsertRows(QModelIndex(), row, row + count - 1));

	for (int i = 0; i < count; ++i) {
		QVector<QVariant> newVector(newLineTemplate);
		tableData.insert(row, newVector);
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