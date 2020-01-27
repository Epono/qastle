#pragma once

#include <QString>
#include <QMetaType>

class Utils
{
public:
	// TODO: handle enums, flags, etc
	static QString getTypeFromStringType(const QMetaType::Type& type, const QString& data = QString());
	static QMetaType::Type getTypeFromStringType(const QString& data);

	// TODO: Template
	//static QVariant valueFromType(const QMetaType::Type& type, const QVariant& value);
};

