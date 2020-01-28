#include "../include/utils.h"

#include <QStringList>
#include <QDebug>

// https://github.com/ncannasse/castle/blob/2468d659eb18198049b55693fa117a93b16252df/cdb/Parser.hx
	//public static function getType(str : String) : Data.ColumnType{
	//return switch (Std.parseInt(str)) {
	//case 0: TId;
	//case 5: TEnum(str.substr(str.indexOf(":") + 1).split(","));
	//case 6: TRef(str.substr(str.indexOf(":") + 1));
	//case 7: TImage;
	//case 8: TList;
	//case 9: TCustom(str.substr(str.indexOf(":") + 1));
	//case 10: TFlags(str.substr(str.indexOf(":") + 1).split(","));
	//case 11: TColor;
	//case 12: TLayer(str.substr(str.indexOf(":") + 1));
	//case 13: TFile;
	//case 14: TTilePos;
	//case 15: TTileLayer;
	//case 16: TDynamic;
	//case 17: TProperties;

QString Utils::getTypeFromStringType(const QMetaType::Type& type, const QString& data) {
	switch (type)
	{
	case QMetaType::QString:
		//case 1: TString;
		return QString("1");
	case QMetaType::Bool:
		//case 2: TBool;
		return QString("2");
	case QMetaType::Int:
		//case 3: TInt;
		return QString("3");
	case QMetaType::Float:
		//case 4: TFloat;
		return QString("4");
	case QMetaType::Double:
		// y'a pas, va pour float
		return QString("4");
	default:
		//default: throw "Unknown type " + str;
		return QString("Unknown type : %1").arg(type);
	}
}

// TODO: add validations
QMetaType::Type Utils::getTypeFromStringType(const QString& data) {
	QStringList list = data.split(":");
	int typeNumber = list[0].toInt();

	QStringList otherData;
	if (list.size() > 1) {
		otherData = list[1].split(",");
	}

	switch (typeNumber) {
	case 1:
		return QMetaType::Type::QString;
	case 2:
		return QMetaType::Type::Bool;
	case 3:
		return QMetaType::Type::Int;
	case 4:
		return QMetaType::Type::Float;
	default:
		qDebug() << QString("[getTypeFromStringType] Type not found: %1").arg(typeNumber);
		//return QMetaType::Type::UnknownType;
		return QMetaType::Type::QCursor;
	}
}