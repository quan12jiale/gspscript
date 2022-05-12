#include "libxlUtils.h"
#include "CppDependBroadcast.h"
#include <QDir>
#include <QFile>
#include <QCommandLineParser>
#include <QCoreApplication>

struct Options
{
	enum ActionType {
		ActionAdd,
		ActionModify,
		ActionDelete
	};

	ActionType action;
	QString dbpath; 
	QString jspath;
};

// 参考了windeployqt的命令行解析思路
bool parseArguments(const QStringList &arguments,
	Options& options)
{
	QCommandLineParser parser;

	QCommandLineOption moduleOption(QStringLiteral("m"),
		QStringLiteral("运行的python模块"),
		QStringLiteral("module"));
	parser.addOption(moduleOption);

	QCommandLineOption actionOption(QStringLiteral("a"),
		QStringLiteral("执行的操作"),
		QStringLiteral("action"));
	parser.addOption(actionOption);

	QCommandLineOption dbpathOption(QStringLiteral("d"),
		QStringLiteral("数据库"),
		QStringLiteral("database"));
	parser.addOption(dbpathOption);

	QCommandLineOption jspathOption(QStringLiteral("j"),
		QStringLiteral("修改项"),
		QStringLiteral("json"));
	parser.addOption(jspathOption);

	if (!parser.parse(arguments))
	{
		qDebug() << parser.errorText();
		return false;
	}
		
	if (!parser.isSet(actionOption))
		return false;
	QString strAction = parser.value(actionOption);
	if (strAction == "add")
	{
		options.action = Options::ActionAdd;
	}
	else if (strAction == "modify")
	{
		options.action = Options::ActionModify;
	}
	else if (strAction == "delete")
	{
		options.action = Options::ActionDelete;
	}
	else
	{
		return false;
	}

	if (!parser.isSet(dbpathOption))
		return false;
	options.dbpath = parser.value(dbpathOption);

	if (!parser.isSet(jspathOption))
		return false;
	options.jspath = parser.value(jspathOption);

	return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);//这一行用于消除"Please instantiate the QApplication object first"警告信息

#if 0
	QRegularExpression sep("\\s+");

	QString batPath = "E:/GTJ202110_Release/resource/json/PC/HJBar/3-addrecord.bat";
	QFileInfo fileInfo(batPath);
	QString batDir = fileInfo.absolutePath();
	QFile file(batPath);
	if (file.open(QIODevice::ReadOnly))
	{
		QMap<QString, QString> replaceMap;
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine();
			line = line.trimmed();
			if (line.startsWith("::")) // 过滤注释行
			{
				continue;
			}

			if (line.startsWith("set ")) // set加空格，避免了包含set的单词
			{
				QStringList replaceList = line.split('=', QString::SkipEmptyParts);
				if (replaceList.size() == 2)
				{
					QStringList tmpList = replaceList.at(0).split(sep, QString::SkipEmptyParts);
					if (tmpList.size() == 2)
					{
						replaceMap.insert('%' + tmpList.at(1) + '%', replaceList.at(1));
					}
				}
				continue;
			}

			for (auto iter = replaceMap.begin(); iter != replaceMap.end(); iter++)
			{
				line.replace(iter.key(), iter.value());
			}

			QStringList arguments = line.split(sep, QString::SkipEmptyParts);
			if (arguments.isEmpty())
			{
				continue;
			}

			Options options;
			if (!parseArguments(arguments, options))
			{
				continue;
			}
			QString dbpath = batDir + '/' + options.dbpath;
			dbpath = QDir::cleanPath(dbpath);

			QString jspath = batDir + '/' + options.jspath;
			jspath = QDir::cleanPath(jspath);

			if (options.action == Options::ActionAdd)
			{
				Amd obj;
				obj.update(dbpath, jspath, dbadd);
			}
			else if (options.action == Options::ActionModify)
			{
				Amd obj;
				obj.update(dbpath, jspath, dbmodify);
			}
			else if (options.action == Options::ActionDelete)
			{
				Amd obj;
				obj.update(dbpath, jspath, dbdelete);
			}
		}
	}
#else
	QHash<QString, int> authors; 
	const QString path = "D:/PCZ/build/build/BuildTools/CppDepend/";
	transformAccount(authors, path);

	QString html;
	const QString db = "C:/Users/quanjl/Downloads/cpd.db";
	GetDataFromHtml* pGetDataObj = new GetDataFromHtml(html, db);
	pGetDataObj->getIgnoreDatasFromDB();
	delete pGetDataObj;
#endif
	system("pause");
    return 0;
}
