#pragma once

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>
#include "../table_model.h"

class SetData : public QUndoCommand
{
public:
	SetData(const QModelIndex& index, const QVariant& value, TableModel* model)
		: QUndoCommand(), m_model(model) {
		m_oldValue = index.data(Qt::DisplayRole);
		m_newValue = value;
		m_row = index.row();
		m_col = index.column();
		setText(QString("Set (%1,%2) to %3").arg(m_col + 1).arg(m_row + 1).arg(m_newValue.toInt()));
	}

	void redo() override
	{
		QModelIndex index = m_model->index(m_row, m_col);
		m_model->m_tableData[m_col][m_row] = m_newValue.toInt();
		m_model->dataChanged(index, index);
	}

	void undo() override
	{
		QModelIndex index = m_model->index(m_row, m_col);
		m_model->m_tableData[m_col][m_row] = m_oldValue.toInt();
		m_model->dataChanged(index, index);
	}

private:
	TableModel* m_model;
	QVariant m_newValue, m_oldValue;
	int m_row, m_col;
};

