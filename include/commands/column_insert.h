#pragma once

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>
#include "../table_model.h"

class ColumnInsert : public QUndoCommand
{
public:
	ColumnInsert(int column, const QastleType type, const QString headerName, TableModel* model)
		: QUndoCommand(), m_model(model) {
		this->m_column = column;
		this->m_type = type;
		this->m_headerName = headerName;
		setText(QString("Insert 1 column of type %1, of name %2 at %3").arg(Utils::getTypeNameFromType(m_type)).arg(m_headerName).arg(m_column));

	}

	void redo() override
	{
		emit(m_model->beginInsertColumns(QModelIndex(), m_column, m_column));

		m_model->addTypeToTemplate(m_column, m_type, m_headerName);
		m_model->addTypeToExistingData(m_column, m_type);

		emit(m_model->endInsertColumns());
	}

	void undo() override
	{
		emit(m_model->beginRemoveColumns(QModelIndex(), m_column, m_column));

		// TODO: Refacto because with undo/redos it will be copy/pasted
		m_model->m_dataModel.remove(m_column, 1);
		m_model->m_newLineTemplate.remove(m_column, 1);
		m_model->m_headers.remove(m_column, 1);

		for (int j = 0; j < m_model->m_tableData.size(); ++j) {
			m_model->m_tableData[j].remove(m_column, 1);
		}

		emit(m_model->endRemoveColumns());
	}


private:
	TableModel* m_model;
	int m_column;
	QastleType m_type;
	QString m_headerName;
};

