#pragma once

#include <QWidget>
#include <QAbstractTableModel>
#include <QUndoStack>
#include <QKeyEvent>
#include <QTableView>


#include "../include/utils.h"

class TableModel : public QAbstractTableModel {

	Q_OBJECT

	friend class CellSet;
	friend class ColumnInsert;
	friend class ColumnRemove;
	friend class ColumnRename;
	friend class RowInsert;
	friend class RowRemove;

private:
	QString m_sheetName;
	// TODO: en faire un type et avoir un vector de ce type ?
	QVector <QastleType> m_dataModel;
	QVector <QVariant> m_newLineTemplate;
	QVector <QString> m_headers;

	QVector <QVector <QVariant> > m_tableData;

	std::unique_ptr<QUndoStack> m_undoStack;

public:
	TableModel(QObject* parent = nullptr);
	~TableModel();

	void addFirstColumnAndRow();

	Qt::ItemFlags flags(const QModelIndex& index) const;

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	// Except this one
	bool insertColumnTyped(int column, const QastleType type, const QString headerName, const QModelIndex& parent = QModelIndex());
	bool insertColumnsTyped(const QVector<QastleType> types, const QVector<QString> headerNames, const QModelIndex& parent = QModelIndex());
	//bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());

	bool insertRowWithData(int row, QVector <QVariant>& data, const QModelIndex& parent = QModelIndex());
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	void setHeaderAtIndex(const int index, const QString newHeaderName);

	// ACCESSORS
	QVector <QastleType> dataModel() const;
	//void setDataModel(const QVector <QMetaType::Type>& dataModel);

	QVector <QVariant> newLineTemplate() const;
	//void setNewLineTemplate(const QVector <QVariant>& newLineTemplate);

	QVector <QString> headers() const;
	//void setHeaders(const QVector <QString>& headers);

	QVector <QVector <QVariant> > tableData() const;
	//void setTableData(const QVector <QVector <QVariant> >& tableData);

	QString sheetName() const;
	void setSheetName(const QString& sheetName);

	QUndoStack* undoStack() const;

	// TEMP
	void addDummyData();

private:
	void addTypeToTemplate(int column, const QastleType type, const QString headerName);
	void addTypeToExistingData(int column, const QastleType type);
};