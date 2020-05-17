#pragma once

#include "../table_model.h"

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>

class ColumnRename : public QUndoCommand
{
public:
	ColumnRename(int column, const QString name, TableModel* model)
		: QUndoCommand(), m_model(model) {
		m_oldValue = model->m_headers[column];
		m_newValue = name;
		m_column = column;
		setText(QString("Rename col %1 from \"%2\" to \"%3\"").arg(m_column).arg(m_oldValue).arg(m_newValue));
	}

	void redo() override
	{
		m_model->m_headers[m_column] = m_newValue;
		m_model->headerDataChanged(Qt::Orientation::Horizontal, m_column, m_column);
	}

	void undo() override
	{
		m_model->m_headers[m_column] = m_oldValue;
		m_model->headerDataChanged(Qt::Orientation::Horizontal, m_column, m_column);
	}

private:
	TableModel* m_model;
	QString m_newValue, m_oldValue;
	int m_column;
};

