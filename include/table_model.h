#pragma once

#include <QtWidgets/QWidget>
#include <QAbstractTableModel>
#include <vector>

class TableModel : public QAbstractTableModel {

	Q_OBJECT

private:
	// TODO: en faire un type et avoir un vector de ce type ?
	QVector <QMetaType::Type> mDataModel;
	QVector <QVariant> mNewLineTemplate;
	QVector <QString> mHeaders;

	QVector <QVector <QVariant> > mTableData;

	void TableModel::addTypeToTemplate(int column, const QMetaType::Type type, const QString headerName);
	void TableModel::addTypeToExistingData(int column, const QMetaType::Type type);

public:
	// OVERRIDE
	TableModel(QObject* parent = nullptr);

	Qt::ItemFlags TableModel::flags(const QModelIndex& index) const;

	int TableModel::rowCount(const QModelIndex& parent = QModelIndex()) const;
	int TableModel::columnCount(const QModelIndex& parent = QModelIndex()) const;

	// Except this one
	bool TableModel::insertColumnTyped(int column, const QMetaType::Type type, const QString headerName, const QModelIndex& parent = QModelIndex());
	//bool TableModel::insertColumns(int column, int count, const QModelIndex& parent = QModelIndex());
	bool TableModel::removeColumns(int column, int count, const QModelIndex& parent = QModelIndex());

	bool TableModel::insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool TableModel::removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

	QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
	QVariant TableModel::data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	// ACCESSORS
	QVector <QMetaType::Type> TableModel::dataModel() const;
	void TableModel::setDataModel(const QVector <QMetaType::Type>& dataModel);

	QVector <QVariant> TableModel::newLineTemplate() const;
	void TableModel::setNewLineTemplate(const QVector <QVariant>& newLineTemplate);

	QVector <QString> TableModel::headers() const;
	void TableModel::setHeaders(const QVector <QString>& headers);

	QVector <QVector <QVariant> > TableModel::tableData() const;
	void TableModel::setTableData(const QVector <QVector <QVariant> >& tableData);
};