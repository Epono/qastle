#pragma once

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>
#include "../table_model.h"

class RowRemove : public QUndoCommand
{
public:
	RowRemove(int row, int count, TableModel* model)
		: QUndoCommand(), m_model(model) {
		m_row = row;
		m_count = count;

		for (int i = 0; i < m_count; i++) {
			QVector <QVariant> currentRow(m_model->m_tableData[m_row + i]);
			m_rowsRemoved.push_back(currentRow);
		}

		setText(QString("Remove %1 row(s) starting from %2").arg(m_count).arg(m_row));
	}

	void redo() override
	{
		emit(m_model->beginRemoveRows(QModelIndex(), m_row, m_row + m_count - 1));

		qDebug() << "allo-;";
		m_model->m_tableData.remove(m_row, m_count);

		emit(m_model->endRemoveRows());
	}

	void undo() override
	{
		emit(m_model->beginInsertRows(QModelIndex(), m_row, m_row + m_count - 1));

		for (int i = 0; i < m_count; i++) {
			m_model->m_tableData.insert(i + m_row, m_rowsRemoved[i]);
		}

		emit(m_model->endInsertRows());
	}

private:
	TableModel* m_model;
	int m_row, m_count;
	QVector <QVector <QVariant> > m_rowsRemoved;
};

