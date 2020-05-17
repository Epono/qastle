#include "../include/utils.h"

#include <QStringList>
#include <QFileInfo>
#include <QDebug>

QString Utils::getTypeIdFromType(const QastleType& type, const QString& data) {
	switch (type)
	{
	case QastleType::TString:
		return QString("1");
	case QastleType::TBool:
		return QString("2");
	case QastleType::TInt:
		return QString("3");
	case QastleType::TFloat:
		return QString("4");
	default:
		// TODO: throw
		return QString("Unknown type : %1").arg(int(type));
	}

	// ?
	//return QString(static_cast<int>(type));
}

QString Utils::getTypeNameFromType(const QastleType& type) {
	switch (type)
	{
	case QastleType::TId:
		return QString("ID");
	case QastleType::TString:
		return QString("String");
	case QastleType::TBool:
		return QString("Boolean");
	case QastleType::TInt:
		return QString("Integer");
	case QastleType::TFloat:
		return QString("Float");
	case QastleType::TEnum:
		return QString("Enum");
	case QastleType::TRef:
		return QString("Reference");
	case QastleType::TImage:
		return QString("Image");
	case QastleType::TList:
		return QString("List");
	case QastleType::TCustom:
		return QString("Custom");
	case QastleType::TFlags:
		return QString("FLags");
	case QastleType::TColor:
		return QString("Color");
	case QastleType::TLayer:
		return QString("Layer");
	case QastleType::TFile:
		return QString("File");
	case QastleType::TTilePos:
		return QString("Tile Pos");
	case QastleType::TTileLayer:
		return QString("Tile Layer");
	case QastleType::TDynamic:
		return QString("Dynamic");
	case QastleType::TProperties:
		return QString("Properties");
	default:
		// TODO: throw
		return QString("Unknown type : %1").arg(int(type));
	}
}

// TODO: add validations
QastleType Utils::getTypeFromStringType(const QString& data) {
	QStringList list = data.split(":");
	int typeNumber = list[0].toInt();

	QStringList otherData;
	if (list.size() > 1) {
		otherData = list[1].split(",");
	}

	switch (typeNumber) {
	case 1:
		return QastleType::TString;
	case 2:
		return QastleType::TBool;
	case 3:
		return QastleType::TInt;
	case 4:
		return QastleType::TFloat;
	default:
		// TODO: throw
		qDebug() << QString("[getTypeFromStringType] Type not found: %1").arg(typeNumber);
		return QastleType::TUnknown;
	}
}

bool Utils::fileExists(const QString& path) {
	return QFileInfo::exists(path) && QFileInfo(path).isFile();
}