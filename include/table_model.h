#pragma once

#include <QWidget>
#include <QAbstractTableModel>
#include <vector>

class TableModel : public QAbstractTableModel {

	Q_OBJECT

private:
	QString mSheetName;
	// TODO: en faire un type et avoir un vector de ce type ?
	QVector <QMetaType::Type> mDataModel;
	QVector <QVariant> mNewLineTemplate;
	QVector <QString> mHeaders;

	QVector <QVector <QVariant> > mTableData;

	void addTypeToTemplate(int column, const QMetaType::Type type, const QString headerName);
	void addTypeToExistingData(int column, const QMetaType::Type type);

public:
	// OVERRIDE
	TableModel(QObject* parent = nullptr);

	Qt::ItemFlags flags(const QModelIndex& index) const;

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const;

	// Except this one
	bool insertColumnTyped(int column, const QMetaType::Type type, const QString headerName, const QModelIndex& parent = QModelIndex());
	//bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());

	bool insertRowWithData(int row, QVector <QVariant>& data, const QModelIndex& parent = QModelIndex());
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	// ACCESSORS
	QVector <QMetaType::Type> dataModel() const;
	//void setDataModel(const QVector <QMetaType::Type>& dataModel);

	QVector <QVariant> newLineTemplate() const;
	//void setNewLineTemplate(const QVector <QVariant>& newLineTemplate);

	QVector <QString> headers() const;
	//void setHeaders(const QVector <QString>& headers);

	QVector <QVector <QVariant> > tableData() const;
	//void setTableData(const QVector <QVector <QVariant> >& tableData);

	QString sheetName() const;
	void setSheetName(const QString& sheetName);

	// TEMP
	void addDummyData();
};