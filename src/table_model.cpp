#include "table_model.h"

#include <QDebug>

// Advanced example?
//QMetaType::Type type = QMetaType::Type::Bool;
//if (type != QMetaType::Type::UnknownType) {
//	void* ptr = QMetaType::create(type);

//	// ... Do things

//	QMetaType::destroy(type, ptr);
//	ptr = 0;
//}

TableModel::TableModel(QObject* parent) {

	this->addTypeToTemplate(0, QMetaType::Type::Bool, "Dead?");
	this->addTypeToTemplate(0, QMetaType::Type::Int, "Age");
	this->addTypeToTemplate(0, QMetaType::Type::Float, "Height");
	this->addTypeToTemplate(0, QMetaType::Type::Double, "Weight");
	this->addTypeToTemplate(0, QMetaType::Type::QString, "Name");

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

bool TableModel::insertColumnTyped(int column, const QMetaType::Type type, const QString headerName, const QModelIndex& parent) {
	emit(this->beginInsertColumns(QModelIndex(), column, column));

	addTypeToTemplate(column, type, headerName);
	addTypeToExistingData(column, type);

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

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
void TableModel::addTypeToTemplate(int column, const QMetaType::Type type, const QString headerName) {
	dataModel.insert(column, type);

	switch (type)
	{
	case QMetaType::QString:
		newLineTemplate.insert(column, QString());
		break;
	case QMetaType::Bool:
		newLineTemplate.insert(column, false);
		break;
	case QMetaType::Int:
		newLineTemplate.insert(column, int(0));
		break;
	case QMetaType::Float:
		newLineTemplate.insert(column, float(0));
		break;
	case QMetaType::Double:
		newLineTemplate.insert(column, double(0));
		break;
	}

	headers.insert(column, headerName);
}

void TableModel::addTypeToExistingData(int column, const QMetaType::Type type) {
	for (int j = 0; j < tableData.size(); ++j) {
		switch (type)
		{
		case QMetaType::QString:
			tableData[j].insert(column, QString());
			break;
		case QMetaType::Bool:
			tableData[j].insert(column, false);
			break;
		case QMetaType::Int:
			tableData[j].insert(column, int(0));
			break;
		case QMetaType::Float:
			tableData[j].insert(column, float(0));
			break;
		case QMetaType::Double:
			tableData[j].insert(column, double(0));
			break;
		}
	}
}