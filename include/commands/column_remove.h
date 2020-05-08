#pragma once

#include <QUndoStack>
#include <QVector>
#include <QVariant>
#include <QModelIndex>
#include <QDebug>
#include "../table_model.h"

class ColumnRemove : public QUndoCommand
{
public:
	ColumnRemove(int column, int count, TableModel* model)
		: QUndoCommand(), m_model(model) {
		m_column = column;
		m_count = count;

		for (auto const& row : m_model->m_tableData) {
			QVector <QVariant> backupRow;
			for (int i = 0; i < m_count; i++) {
				backupRow.push_back(row[m_column + i]);
			}
			m_tableDataBefore.push_back(backupRow);
		}
		for (int i = 0; i < m_count; i++) {
			m_dataModelBefore.push_back(m_model->m_dataModel[m_column + i]);
		}
		for (int i = 0; i < m_count; i++) {
			m_headersBefore.push_back(m_model->m_headers[m_column + i]);
		}

		setText(QString("Remove %1 column(s) starting from %2").arg(m_count).arg(m_column));
	}

	void redo() override
	{
		emit(m_model->beginRemoveColumns(QModelIndex(), m_column, m_column + m_count - 1));

		// TODO: Refacto because with undo/redos it will be copy/pasted
		m_model->m_dataModel.remove(m_column, m_count);
		m_model->m_newLineTemplate.remove(m_column, m_count);
		m_model->m_headers.remove(m_column, m_count);

		for (int j = 0; j < m_model->m_tableData.size(); ++j) {
			m_model->m_tableData[j].remove(m_column, m_count);
		}

		emit(m_model->endRemoveColumns());
	}

	void undo() override
	{
		emit(m_model->beginInsertColumns(QModelIndex(), m_column, m_column + m_count - 1));

		for (int i = 0; i < m_count; i++) {
			m_model->addTypeToTemplate(m_column + i, m_dataModelBefore[i], m_headersBefore[i]);
			m_model->addTypeToExistingData(m_column + i, m_dataModelBefore[i]);
		}

		for (int i = 0; i < m_model->m_tableData.size(); i++) {
			auto row = m_model->m_tableData[i];
			const auto& backupRow = m_tableDataBefore[i];
			for (int j = 0; j < m_count; j++) {
				row.insert(m_column + j, backupRow[j]);
			}
		}

		emit(m_model->endInsertColumns());
	}


private:
	TableModel* m_model;
	int m_column, m_count;
	QVector <QVector <QVariant> > m_tableDataBefore;
	QVector <QastleType> m_dataModelBefore;
	QVector <QString> m_headersBefore;
};

