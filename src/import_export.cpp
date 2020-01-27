#include "../include/import_export.h"
#include "../include/utils.h"

#include <QFile>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

// TODO: add bool for .dat vs .json

// TODO: return array of tableModel*? also tabNames
TableModel* ImportExport::loadFromJson(const QString& filename) {

	QFile file(filename);

	// TODO: exception instead of return?
	// QIODevice::Text only if *.json?
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return nullptr;

	QByteArray fileData = file.readAll();

	QJsonDocument jsonDocument(QJsonDocument::fromJson(fileData));

	// TODO: validate?
	// https://stackoverflow.com/questions/18624701/what-support-is-there-for-json-schema-in-qt-c-c

	QJsonObject rootObject = jsonDocument.object();

	// TODO
	QJsonArray customTypesArray = rootObject["customTypes"].toArray();

	// TODO
	bool isCompressed = rootObject["compress"].toBool();

	// TODO: nettoyer si erreur ?
	TableModel* model = new TableModel();

	QJsonArray sheetsArray = rootObject["sheets"].toArray();
	//for (int i = 0; i < sheetsArray.size(); ++i) {
	for (int i = 0; i < 1; ++i) { // TODO
		QJsonObject sheetObject = sheetsArray[i].toObject();

		QJsonArray separatorsArray = sheetObject["separators"].toArray();
		QJsonObject propsObject = sheetObject["props"].toObject();

		QString sheetName = sheetObject["name"].toString();
		model->setSheetName(sheetName);

		// COLUMNS
		QJsonArray columnsArray = sheetObject["columns"].toArray();

		// TODO: Load in a new sheet to avoid conflicts
		model->removeColumns(0, 100);
		for (int j = 0; j < columnsArray.size(); ++j) {
			QJsonObject columnObject = columnsArray[j].toObject();
			QString headerName(columnObject["name"].toString());
			QMetaType::Type type(Utils::getTypeFromStringType(columnObject["typeStr"].toString()));

			model->insertColumnTyped(j, type, headerName);
		}

		// ROWS
		QJsonArray linesArray = sheetObject["lines"].toArray();
		for (int j = 0; j < linesArray.size(); ++j) {
			QJsonObject lineObject = linesArray[j].toObject();
			QVector<QVariant> data;

			for (int k = 0; k < model->dataModel().size(); ++k) {
				QMetaType::Type type = model->dataModel()[k];
				QString headerName = model->headers()[k];

				switch (type)
				{
				case QMetaType::QString:
					data.append(lineObject[headerName].toString());
					break;
				case QMetaType::Bool:
					data.append(lineObject[headerName].toBool());
					break;
				case QMetaType::Int:
					data.append(lineObject[headerName].toInt());
					break;
				case QMetaType::Float:
					data.append(lineObject[headerName].toDouble());
					break;
				case QMetaType::Double:
					data.append(lineObject[headerName].toDouble());
					break;
				}
			}

			model->insertRowWithData(j, data);
		}
	}

	return model;
}

bool ImportExport::saveToJson(const QString& filename, const TableModel* const model, const QVector<QString>& tabNames, const bool doCompress) {

	// TODO: JSON or binary
	QFile file(QStringLiteral("data.json"));

	if (!file.open(QIODevice::WriteOnly)) {
		qWarning("Couldn't open data file.");
		return false;
	}

	// TODO: support multiple sheets
	QJsonArray sheetsArray;
	QJsonObject sheetObject;

	sheetObject["name"] = tabNames[0];
	sheetObject["props"] = QJsonObject();
	sheetObject["separators"] = QJsonArray();

	QJsonArray columnsArray;
	for (int i = 0; i < model->dataModel().size(); ++i) {
		QJsonObject columnObject;

		// TODO: refacto pour éviter 12000 appels de fonction ? inline
		// TODO: gérer le param "display" (pour les int/float? notamment) (juste pour intéropérabilité avec cdb, osef un peu sinon)
		QMetaType::Type type = model->dataModel()[i];
		columnObject["typeStr"] = Utils::getTypeFromStringType(type);
		columnObject["name"] = model->headers()[i];
		columnsArray.append(columnObject);
	}

	QJsonArray linesArray;

	for (const QVector<QVariant>& line : model->tableData()) {
		QJsonObject lineObject;
		for (int i = 0; i < model->dataModel().size(); ++i) {
			// REFACTO
			QMetaType::Type type = model->dataModel()[i];
			switch (type)
			{
			case QMetaType::QString:
				lineObject[model->headers()[i]] = line[i].toString();
				break;
			case QMetaType::Bool:
				lineObject[model->headers()[i]] = line[i].toBool();
				break;
			case QMetaType::Int:
				lineObject[model->headers()[i]] = line[i].toInt();
				break;
			case QMetaType::Float:
				lineObject[model->headers()[i]] = line[i].toFloat();
				break;
			case QMetaType::Double:
				lineObject[model->headers()[i]] = line[i].toDouble();
				break;
			}
			//
		}

		linesArray.append(lineObject);
	}

	sheetObject["columns"] = columnsArray;
	sheetObject["lines"] = linesArray;

	sheetsArray.append(sheetObject);

	QJsonObject rootObject;
	rootObject["sheets"] = sheetsArray;
	rootObject["customTypes"] = QJsonArray();
	rootObject["compress"] = doCompress;

	QJsonDocument jsonDocument(rootObject);

	// TODO: add bool to QJsonDocument::Compact instead of QJsonDocument::Indented
	file.write(jsonDocument.toJson());

	return true;
}