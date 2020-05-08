#pragma once

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>
#include "../table_model.h"

class RowInsert : public QUndoCommand
{
public:
	RowInsert(int row, int count, TableModel* model)
		: QUndoCommand(), m_model(model) {
		m_row = row;
		m_count = count;
		setText(QString("Insert %1 row(s) starting from %2").arg(m_count).arg(m_row));
	}

	void redo() override
	{
		emit(m_model->beginInsertRows(QModelIndex(), m_row, m_row + m_count - 1));

		for (int i = 0; i < m_count; ++i) {
			QVector<QVariant> newVector(m_model->m_newLineTemplate);
			m_model->m_tableData.insert(m_row + i, newVector);
		}

		emit(m_model->endInsertRows());
	}

	void undo() override
	{
		emit(m_model->beginRemoveRows(QModelIndex(), m_row, m_row + m_count - 1));

		m_model->m_tableData.remove(m_row, m_count);

		emit(m_model->endRemoveRows());
	}

private:
	TableModel* m_model;
	int m_row, m_count;
};

