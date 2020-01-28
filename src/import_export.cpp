#include "../include/import_export.h"
#include "../include/table_model.h"
#include "../include/utils.h"

#include <QFile>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

// TODO: add bool for .dat vs .json

// TODO: return array of tableModel*? also tabNames
QVector<TableModel*> ImportExport::loadFromJson(const QString& filename) {

	QFile file(filename);

	// TODO: exception instead of return?
	// QIODevice::Text only if *.json?
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return QVector<TableModel*>();

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
	QJsonArray sheetsArray = rootObject["sheets"].toArray();
	QVector<TableModel*> tableModels;

	for (int i = 0; i < sheetsArray.size(); ++i) {
		QJsonObject sheetObject = sheetsArray[i].toObject();
		TableModel* tableModel = new TableModel();

		QJsonArray separatorsArray = sheetObject["separators"].toArray();
		QJsonObject propsObject = sheetObject["props"].toObject();

		QString sheetName = sheetObject["name"].toString();
		tableModel->setSheetName(sheetName);

		// COLUMNS
		QJsonArray columnsArray = sheetObject["columns"].toArray();

		// TODO: Load in a new sheet to avoid conflicts
		tableModel->removeColumns(0, 100);
		for (int j = 0; j < columnsArray.size(); ++j) {
			QJsonObject columnObject = columnsArray[j].toObject();
			QString headerName(columnObject["name"].toString());
			QMetaType::Type type(Utils::getTypeFromStringType(columnObject["typeStr"].toString()));

			tableModel->insertColumnTyped(j, type, headerName);
		}

		// ROWS
		QJsonArray linesArray = sheetObject["lines"].toArray();
		for (int j = 0; j < linesArray.size(); ++j) {
			QJsonObject lineObject = linesArray[j].toObject();
			QVector<QVariant> data;

			for (int k = 0; k < tableModel->dataModel().size(); ++k) {
				QMetaType::Type type = tableModel->dataModel()[k];
				QString headerName = tableModel->headers()[k];

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
				case QMetaType::Type::QCursor:
					data.append(QString("Not supported"));
				default:
					// TODO
					break;
				}
			}

			tableModel->insertRowWithData(j, data);
		}

		tableModels.append(tableModel);
	}

	return tableModels;
}

bool ImportExport::saveToJson(const QString& filename, const QVector<TableModel*> tableModels, const bool doCompress) {

	// TODO: JSON or binary
	QFile file(filename);

	if (!file.open(QIODevice::WriteOnly)) {
		qWarning("Couldn't open data file.");
		return false;
	}

	// TODO: support multiple sheets
	QJsonArray sheetsArray;

	for (TableModel* model : tableModels) {
		QJsonObject sheetObject;

		sheetObject["name"] = model->sheetName();
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
	}

	QJsonObject rootObject;
	rootObject["sheets"] = sheetsArray;
	rootObject["customTypes"] = QJsonArray();
	rootObject["compress"] = doCompress;

	QJsonDocument jsonDocument(rootObject);

	// TODO: add bool to QJsonDocument::Compact instead of QJsonDocument::Indented
	file.write(jsonDocument.toJson());

	return true;
}