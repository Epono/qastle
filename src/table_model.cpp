#include "../include/table_model.h"

#include <QDebug>

// Advanced example?
//QMetaType::Type type = QMetaType::Type::Bool;
//if (type != QMetaType::Type::UnknownType) {
//	void* ptr = QMetaType::create(type);

//	// ... Do things

//	QMetaType::destroy(type, ptr);
//	ptr = 0;
//}

void TableModel::addDummyData() {
	insertColumnTyped(0, QMetaType::Type::Bool, "Dead?");
	insertColumnTyped(0, QMetaType::Type::Int, "Age");
	insertColumnTyped(0, QMetaType::Type::Float, "Height");
	insertColumnTyped(0, QMetaType::Type::Double, "Weight");
	insertColumnTyped(0, QMetaType::Type::QString, "Name");

	for (int i = 0; i < 10; ++i) {
		QVector<QVariant> tempVector(mNewLineTemplate);
		mTableData.push_back(tempVector);
	}
}

TableModel::TableModel(QObject* parent) {
	// TODO: make a real "id" column
	insertColumnTyped(0, QMetaType::Type::Int, "ID");

	QVector<QVariant> tempVector(mNewLineTemplate);
	mTableData.push_back(tempVector);
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const {
	return (QAbstractItemModel::flags(index) | Qt::ItemFlag::ItemIsEditable) & (~Qt::ItemFlag::ItemIsUserCheckable);
}

int TableModel::rowCount(const QModelIndex& parent) const {
	return mTableData.size();
}

int TableModel::columnCount(const QModelIndex& parent) const {
	if (mTableData.size() > 0) {
		return mTableData[0].size();
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
			return QString("%1\n(%2)").arg(mHeaders[section]).arg(QMetaType::typeName(mDataModel[section]));
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
		mTableData[index.row()][index.column()] = value;
		return true;
	}

	return false;
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	if (role == Qt::ItemDataRole::DisplayRole) {
		return mTableData[index.row()][index.column()];
	}
	else if (role == Qt::ItemDataRole::EditRole) {
		return mTableData[index.row()][index.column()];
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
	// Hack to allow removeColumns(0, 100000);
	if (column + count > mDataModel.size()) {
		count = mDataModel.size() - column;
	}
	emit(this->beginRemoveColumns(QModelIndex(), column, column + count - 1));

	mDataModel.remove(column, count);
	mNewLineTemplate.remove(column, count);
	mHeaders.remove(column, count);

	for (int j = 0; j < mTableData.size(); ++j) {
		mTableData[j].remove(column, count);
	}

	emit(this->endRemoveColumns());
	return true;
}

bool TableModel::insertRowWithData(int row, QVector <QVariant>& data, const QModelIndex& parent) {
	emit(this->beginInsertRows(QModelIndex(), row, row));

	QVector<QVariant> newVector(data);
	mTableData.insert(row, newVector);

	emit(this->endInsertRows());
	return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent) {
	emit(this->beginInsertRows(QModelIndex(), row, row + count - 1));

	for (int i = 0; i < count; ++i) {
		QVector<QVariant> newVector(mNewLineTemplate);
		mTableData.insert(row, newVector);
	}

	emit(this->endInsertRows());
	return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent) {
	emit(this->beginRemoveRows(QModelIndex(), row, row + count - 1));

	mTableData.remove(row, count);

	emit(this->endRemoveRows());
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
void TableModel::addTypeToTemplate(int column, const QMetaType::Type type, const QString headerName) {
	mDataModel.insert(column, type);

	switch (type)
	{
	case QMetaType::QString:
		mNewLineTemplate.insert(column, QString());
		break;
	case QMetaType::Bool:
		mNewLineTemplate.insert(column, false);
		break;
	case QMetaType::Int:
		mNewLineTemplate.insert(column, int(0));
		break;
	case QMetaType::Float:
		mNewLineTemplate.insert(column, float(0));
		break;
	case QMetaType::Double:
		mNewLineTemplate.insert(column, double(0));
		break;
	default:
		qDebug() << QString("[addTypeToTemplate] Type not found: %1").arg(type);
	}

	mHeaders.insert(column, headerName);
}

void TableModel::addTypeToExistingData(int column, const QMetaType::Type type) {
	for (int j = 0; j < mTableData.size(); ++j) {
		switch (type)
		{
		case QMetaType::QString:
			mTableData[j].insert(column, QString());
			break;
		case QMetaType::Bool:
			mTableData[j].insert(column, false);
			break;
		case QMetaType::Int:
			mTableData[j].insert(column, int(0));
			break;
		case QMetaType::Float:
			mTableData[j].insert(column, float(0));
			break;
		case QMetaType::Double:
			mTableData[j].insert(column, double(0));
			break;
		}
	}
}

// ACCESSORS
QVector <QMetaType::Type> TableModel::dataModel() const {
	return mDataModel;
}

//void TableModel::setDataModel(const QVector <QMetaType::Type>& dataModel) {
//	mDataModel = dataModel;
//}

QVector <QVariant> TableModel::newLineTemplate() const {
	return mNewLineTemplate;
}

//void TableModel::setNewLineTemplate(const QVector <QVariant>& newLineTemplate) {
//	mNewLineTemplate = newLineTemplate;
//}

QVector <QString> TableModel::headers() const {
	return mHeaders;
}

//void TableModel::setHeaders(const QVector <QString>& headers) {
//	mHeaders = headers;
//}

QVector <QVector <QVariant> > TableModel::tableData() const {
	return mTableData;
}

//void TableModel::setTableData(const QVector <QVector <QVariant> >& tableData) {
//	mTableData = tableData;
//}

QString TableModel::sheetName() const {
	return mSheetName;
}

void TableModel::setSheetName(const QString& sheetName) {
	mSheetName = sheetName;
}