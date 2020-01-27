#pragma once

#include <QString>
#include "table_model.h"

class ImportExport
{
public:
	static TableModel* ImportExport::loadFromJson(const QString& filename);
	static bool ImportExport::saveToJson(const QString& filename, const TableModel* const data, const QVector<QString>& tabNames, const bool doCompress);
};

