#pragma once

#include <QString>
#include <QMetaType>

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

//public function getConvFunction(old : ColumnType, t : ColumnType, ? custom : { t: ColumnType, f : Dynamic -> Dynamic }) {
//	var conv : Dynamic->Dynamic = null;
//	if (Type.enumEq(old, t)) {
//		if (custom != null && Type.enumEq(old, custom.t))
//			return { f: custom.f };
//		return { f: null };
//	}
//	switch ([old, t]) {
//	case[TInt, TFloat]:
//		// nothing
//	case[TId | TRef(_) | TLayer(_), TString]:
//		// nothing
//	case[TString, (TId | TRef(_) | TLayer(_))]:
//		var r_invalid = ~/ [^ A - Za - z0 - 9_] / g;
//		conv = function(r:String) return r_invalid.replace(r, "_");
//	case[TBool, (TInt | TFloat)]:
//		conv = function(b) return b ? 1 : 0;
//	case[TString, TInt]:
//		conv = Std.parseInt;
//	case[TString, TFloat]:
//		conv = function(str) { var f = Std.parseFloat(str); return Math.isNaN(f) ? null : f; }
//	case[TString, TBool]:
//		conv = function(s) return s != "";
//	case[TString, TEnum(values)]:
//		var map = new Map();
//		for (i in 0...values.length)
//			map.set(values[i].toLowerCase(), i);
//		conv = function(s:String) return map.get(s.toLowerCase());
//	case[TFloat, TInt]:
//		conv = function(v) return Std.int(v);
//	case[(TInt | TFloat | TBool), TString]:
//		conv = Std.string;
//	case[(TFloat | TInt), TBool]:
//		conv = function(v : Float) return v != 0;
//	case[TEnum(values1), TEnum(values2)]:
//		var map = [];
//		for (p in makePairs([for (i in 0...values1.length) { name: values1[i], i : i }], [for (i in 0...values2.length) { name: values2[i], i : i }])) {
//			if (p.b == null) continue;
//			map[p.a.i] = p.b.i;
//		}
//		conv = function(i) return map[i];
//	case[TEnum(values), TString]:
//		conv = function(i) return values[i];
//	case[TFlags(values1), TFlags(values2)]:
//		var map : Array<Null<Int>> = [];
//		for (p in makePairs([for (i in 0...values1.length) { name: values1[i], i : i }], [for (i in 0...values2.length) { name: values2[i], i : i }])) {
//			if (p.b == null) continue;
//			map[p.a.i] = p.b.i;
//		}
//		conv = function(i) {
//			var out = 0;
//			var k = 0;
//			while (i >= 1 << k) {
//				if (map[k] != null && i & (1 << k) != 0)
//					out |= 1 << map[k];
//				k++;
//			}
//			return out;
//		};
//	case[TInt, TEnum(values)]:
//		conv = function(i) return if (i < 0 || i >= values.length) null else i;
//	case[TEnum(values), TInt]:
//		// nothing
//	case[TFlags(values), TInt]:
//		// nothing
//		case[TEnum(val1), TFlags(val2)] if (Std.string(val1) == Std.string(val2)) :
//			conv = function(i) return 1 << i;
//		case[TInt, TColor] | [TColor, TInt]:
//			conv = function(i) return i;
//		case[TList, TProperties]:
//			conv = function(l) return l[0];
//		case[TProperties, TList]:
//			conv = function(p) return Reflect.fields(p).length == 0 ? [] : [p] ;
//		default:
//			return null;
//	}
//	return { f: conv };
//}

enum class QastleType {
	TId = 0,
	TString = 1,
	TBool = 2,
	TInt = 3,
	TFloat = 4,
	TEnum = 5,
	TRef = 6,
	TImage = 7,
	TList = 8,
	TCustom = 9,
	TFlags = 10,
	TColor = 11,
	TLayer = 12,
	TFile = 13,
	TTilePos = 14,
	TTileLayer = 15,
	TDynamic = 16,
	TProperties = 17,
	TUnknown = 100
};

class Utils
{
public:
	// TODO: handle enums, flags, etc
	static QString getTypeIdFromType(const QastleType& type, const QString& data = QString());
	static QString getTypeNameFromType(const QastleType& type);
	static QastleType getTypeFromStringType(const QString& data);

	// TODO: Template
	//static QVariant valueFromType(const QMetaType::Type& type, const QVariant& value);

	static bool fileExists(const QString& path);
};

