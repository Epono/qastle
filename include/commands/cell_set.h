#pragma once

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>
#include "../table_model.h"

class CellSet : public QUndoCommand
{
public:
	CellSet(const QModelIndex& index, const QVariant value, TableModel* model)
		: QUndoCommand(), m_model(model) {
		m_oldValue = index.data(Qt::DisplayRole);
		m_newValue = value;
		m_row = index.row();
		m_column = index.column();
		type = m_model->m_dataModel[m_column];
		setText(QString("Set (%1,%2) to %3").arg(m_column + 1).arg(m_row + 1).arg(m_newValue.toString()));
	}

	void redo() override
	{
		changeData(m_newValue);
	}

	void undo() override
	{
		changeData(m_oldValue);
	}

	void changeData(const QVariant& value) {
		QModelIndex index = m_model->index(m_row, m_column);
		switch (type)
		{
		case QastleType::TString:
			m_model->m_tableData[m_row][m_column] = value.toString();
			break;
		case QastleType::TBool:
			m_model->m_tableData[m_row][m_column] = value.toBool();
			break;
		case QastleType::TInt:
			m_model->m_tableData[m_row][m_column] = value.toInt();
			break;
		case QastleType::TFloat:
			m_model->m_tableData[m_row][m_column] = value.toFloat();
			break;
		default:
			// TODO: 
			m_model->m_tableData[m_row][m_column] = value.toString();
			//qDebug() << QString("Unknown type : %1").arg(int(type));
			break;
		}
		m_model->dataChanged(index, index);
	}

private:
	TableModel* m_model;
	QastleType type;
	QVariant m_newValue, m_oldValue;
	int m_row, m_column;
};

