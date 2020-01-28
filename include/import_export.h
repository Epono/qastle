#pragma once

#include "table_model.h"

#include <QString>

class ImportExport
{
public:
	static QVector<TableModel*> ImportExport::loadFromJson(const QString& filename);
	static bool ImportExport::saveToJson(const QString& filename, const QVector<TableModel*> tableModels, const bool doCompress);
};

