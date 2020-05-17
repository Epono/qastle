#include "../include/qastle.h"

#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QApplication::setApplicationName("qastledb");
	QApplication::setApplicationVersion("0.1");
	QApplication::setApplicationDisplayName("QastleDB");
	QApplication::setWindowIcon(QIcon("Resources/icon.png"));

	QCommandLineParser parser;
	parser.setApplicationDescription("Simple database (using sheets) for game developers");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("db", "The JSON file to load.");

	// A boolean option with a single name (-p)
	QCommandLineOption showProgressOption("p", "Show progress during copy");
	parser.addOption(showProgressOption);

	// A boolean option with multiple names (-f, --force)
	QCommandLineOption forceOption(QStringList() << "f" << "force",
		"Overwrite existing files.");
	parser.addOption(forceOption);

	// An option with a value
	QCommandLineOption targetDirectoryOption(QStringList() << "t" << "target-directory",
		"Copy all source files into <directory>.",
		"directory");
	parser.addOption(targetDirectoryOption);

	// Process the actual command line arguments given by the user
	parser.process(app);

	bool showProgress = parser.isSet(showProgressOption);
	bool force = parser.isSet(forceOption);
	QString targetDir = parser.value(targetDirectoryOption);

	const QStringList args = parser.positionalArguments();
	// source is args.at(0), destination is args.at(1)

	QString db_filename;
	if (args.count() == 1)
	{
		db_filename = args.at(0);
	}

	Qastle qastleWindow(db_filename);
	qastleWindow.show();
	return app.exec();
}
