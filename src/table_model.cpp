#include "../include/table_model.h"

#include "../include/commands/cell_set.h"
#include "../include/commands/column_insert.h"
#include "../include/commands/column_remove.h"
#include "../include/commands/column_rename.h"
#include "../include/commands/row_insert.h"
#include "../include/commands/row_remove.h"

#include "../include/qastle.h"

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
	insertColumnTyped(0, QastleType::TBool, "Dead?");
	insertColumnTyped(0, QastleType::TInt, "Age");
	insertColumnTyped(0, QastleType::TFloat, "Height");
	insertColumnTyped(0, QastleType::TString, "Name");

	for (int i = 0; i < 10; ++i) {
		QVector<QVariant> tempVector(m_newLineTemplate);
		m_tableData.push_back(tempVector);
	}
}

void TableModel::addFirstColumnAndRow() {
	insertColumnTyped(0, QastleType::TInt, "ID");

	QVector<QVariant> tempVector(m_newLineTemplate);
	m_tableData.push_back(tempVector);

	m_undoStack->clear();
}

TableModel::TableModel(QObject* parent) {
	m_sheetName = QString("New sheet");
	m_undoStack = std::make_unique<QUndoStack>(this);
}

TableModel::~TableModel() = default;

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const {
	return (QAbstractItemModel::flags(index) | Qt::ItemFlag::ItemIsEditable) & (~Qt::ItemFlag::ItemIsUserCheckable);
}

int TableModel::rowCount(const QModelIndex& parent) const {
	return m_tableData.size();
}

int TableModel::columnCount(const QModelIndex& parent) const {
	if (m_tableData.size() > 0) {
		return m_tableData[0].size();
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
			return QString("%1\n(%2)").arg(m_headers[section]).arg(Utils::getTypeNameFromType(m_dataModel[section]));
		}
		return QVariant();
	}
}

// https://stackoverflow.com/questions/55855284/qabstracttablemodel-editing-without-clearing-previous-data-in-cell
bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
	if (role == Qt::EditRole) {
		if (!checkIndex(index)) {
			return false;
		}
		// Undo/Redo Stack
		m_undoStack->push(new CellSet(index, value, this));
		return true;
	}

	return false;
}

QVariant TableModel::data(const QModelIndex& index, int role) const {
	if (role == Qt::ItemDataRole::DisplayRole) {
		return m_tableData[index.row()][index.column()];
	}
	else if (role == Qt::ItemDataRole::EditRole) {
		return m_tableData[index.row()][index.column()];
	}
	return QVariant();
}

// Import version
bool TableModel::insertColumnsTyped(const QVector<QastleType> types, const QVector<QString> headerNames, const QModelIndex& parent) {
	emit(this->beginInsertColumns(QModelIndex(), 0, headerNames.size()));

	for (int i = 0; i < headerNames.size(); ++i) {
		addTypeToTemplate(i, types[i], headerNames[i]);
		addTypeToExistingData(i, types[i]);
	}

	emit(this->endInsertColumns());
	return true;
}

// Classic version
bool TableModel::insertColumnTyped(int column, const QastleType type, const QString headerName, const QModelIndex& parent) {

	// OLD
	//emit(this->beginInsertColumns(QModelIndex(), column, column));
	//addTypeToTemplate(column, type, headerName);
	//addTypeToExistingData(column, type);
	//emit(this->endInsertColumns());

	// Undo/Redo Stack
	m_undoStack->push(new ColumnInsert(column, type, headerName, this));

	return true;
}

bool TableModel::removeColumns(int column, int count, const QModelIndex& parent) {
	// Hack to allow removeColumns(0, 100000);
	if (column + count > m_dataModel.size()) {
		count = m_dataModel.size() - column;
	}
	if (count == 0) {
		return true;
	}

	// OLD
	//emit(this->beginRemoveColumns(QModelIndex(), column, column + count - 1));
	//m_dataModel.remove(column, count);
	//m_newLineTemplate.remove(column, count);
	//m_headers.remove(column, count);

	//for (int j = 0; j < m_tableData.size(); ++j) {
	//	m_tableData[j].remove(column, count);
	//}
	//emit(this->endRemoveColumns());

	// Undo/Redo Stack
	m_undoStack->push(new ColumnRemove(column, count, this));

	return true;
}

// Import version
bool TableModel::insertRowWithData(int row, QVector <QVariant>& data, const QModelIndex& parent) {
	emit(this->beginInsertRows(QModelIndex(), row, row));

	QVector<QVariant> newVector(data);
	m_tableData.insert(row, newVector);

	emit(this->endInsertRows());
	return true;
}

// Classic version
bool TableModel::insertRows(int row, int count, const QModelIndex& parent) {
	// OLD
	//emit(this->beginInsertRows(QModelIndex(), row, row + count - 1));
	//for (int i = 0; i < count; ++i) {
	//	QVector<QVariant> newVector(m_newLineTemplate);
	//	m_tableData.insert(row, newVector);
	//}
	//emit(this->endInsertRows());

	// Undo/Redo Stack
	m_undoStack->push(new RowInsert(row, count, this));

	return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent) {
	// OLD
	//emit(this->beginRemoveRows(QModelIndex(), row, row + count - 1));
	//m_tableData.remove(row, count);
	//emit(this->endRemoveRows());

	// Undo/Redo Stack
	m_undoStack->push(new RowRemove(row, count, this));

	return true;
}

void TableModel::setHeaderAtIndex(const int index, const QString newHeaderName) {
	// TODO: check in range

	// OLD
	//m_headers[index] = newHeaderName;

	// Undo/Redo Stack
	m_undoStack->push(new ColumnRename(index, newHeaderName, this));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE
void TableModel::addTypeToTemplate(int column, const QastleType type, const QString headerName) {
	m_dataModel.insert(column, type);

	switch (type)
	{
	case QastleType::TString:
		m_newLineTemplate.insert(column, QString());
		break;
	case QastleType::TBool:
		m_newLineTemplate.insert(column, false);
		break;
	case QastleType::TInt:
		m_newLineTemplate.insert(column, int(0));
		break;
	case QastleType::TFloat:
		m_newLineTemplate.insert(column, float(0));
		break;
	default:
		// TODO
		qDebug() << QString("[addTypeToTemplate] Type not found: %1").arg((int)type);
		break;
	}

	m_headers.insert(column, headerName);
}

void TableModel::addTypeToExistingData(int column, const QastleType type) {
	for (int j = 0; j < m_tableData.size(); ++j) {
		switch (type)
		{
		case QastleType::TString:
			m_tableData[j].insert(column, QString());
			break;
		case QastleType::TBool:
			m_tableData[j].insert(column, false);
			break;
		case QastleType::TInt:
			m_tableData[j].insert(column, int(0));
			break;
		case QastleType::TFloat:
			m_tableData[j].insert(column, float(0));
			break;
		}
	}
}

// ACCESSORS
QVector <QastleType> TableModel::dataModel() const {
	return m_dataModel;
}

//void TableModel::setDataModel(const QVector <QastleType>& dataModel) {
//	mDataModel = dataModel;
//}

QVector <QVariant> TableModel::newLineTemplate() const {
	return m_newLineTemplate;
}

//void TableModel::setNewLineTemplate(const QVector <QVariant>& newLineTemplate) {
//	mNewLineTemplate = newLineTemplate;
//}

QVector <QString> TableModel::headers() const {
	return m_headers;
}

//void TableModel::setHeaders(const QVector <QString>& headers) {
//	mHeaders = headers;
//}

QVector <QVector <QVariant> > TableModel::tableData() const {
	return m_tableData;
}

//void TableModel::setTableData(const QVector <QVector <QVariant> >& tableData) {
//	mTableData = tableData;
//}

QString TableModel::sheetName() const {
	return m_sheetName;
}

void TableModel::setSheetName(const QString& sheetName) {
	m_sheetName = sheetName;
}

QUndoStack* TableModel::undoStack() const {
	return m_undoStack.get();
}