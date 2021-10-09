#include "GTJRunnable.h"

// 定义全局变量，用于发生异常时定位文件
QString g_strGSPFile = QString();

QList<Options> modifySingleGSP(bool bDirectModify);


void handleGIPFile(const QString& sExtractFile, const QList<Options>& lstOption)
{
	// 解压路径
	QString sDestFolder = QCoreApplication::applicationDirPath();
	sDestFolder += "/TempFolder/" + QFileInfo(sExtractFile).baseName() + "/";

	// 创建解压目录
	createDir(sDestFolder);
	QStringList pFiles;
	bool bResult = GMCZipFile::extractFiles(sExtractFile, sDestFolder, &pFiles);//解压
	if (bResult)
	{
		QStringList oFiles;
		for (const QString& strGSPFile : pFiles)
		{
			if (strGSPFile.endsWith("RegionRule_Calc.GSP", Qt::CaseInsensitive))
			{
				g_strGSPFile = strGSPFile;
				for (const Options& opt : lstOption)
				{
					if (opt.action == Options::ActionAdd)
					{
						Amd obj;
						obj.update(strGSPFile, opt.jspath, opt.dbname, dbadd);
					}
					else if (opt.action == Options::ActionModify)
					{
						Amd obj;
						obj.update(strGSPFile, opt.jspath, opt.dbname, dbmodify);
					}
					else if (opt.action == Options::ActionDelete)
					{
						Amd obj;
						obj.update(strGSPFile, opt.jspath, opt.dbname, dbdelete);
					}
				}
			}
			else if (strGSPFile.endsWith("RegionRule_Model.GDB", Qt::CaseInsensitive))
			{
				//QProcess process;
			}
			else if (sameStr(extractFileExt(strGSPFile), ".xml"))
			{
				oFiles << strGSPFile;
			}
		}

		deleteFile(sExtractFile);//删除源文件
		//直接压缩pFiles会导致没有文件夹,所以定义oFiles,并向oFiles中传入MDB文件夹
		oFiles << sDestFolder + "MDB";
		bResult = GMCZipFile::compressFiles(oFiles, QString(), sExtractFile);//压缩
	}

	// 删除解压目录
	//deleteTree(sDestFolder);
}

// 参考了windeployqt的命令行解析思路
bool parseArguments(const QStringList &arguments,
	Options& options)
{
	QCommandLineParser parser;
	// 解析长度超过一个字符的选项，如dbname
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption dbnameOption(QStringLiteral("dbname"),
		QStringLiteral("数据库名称，如Business，BQCalcRule"),
		QStringLiteral("databaseName"));
	parser.addOption(dbnameOption);

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

	if (!parser.isSet(dbnameOption))
		return false;
	options.dbname = parser.value(dbnameOption);

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

QList<Options> modifySingleGSP(bool bSingleGSP) // 默认为true
{
	QList<Options> lstOption;
	QRegularExpression sep("\\s+");

	// !!! bat脚本路径
	QString batPath = "E:/GrowingCaulk/resource/json/PC/PCWall_Caulk/gsp/addGSPRecord.bat";
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

			if (bSingleGSP)
			{
				if (options.action == Options::ActionAdd)
				{
					Amd obj;
					obj.update(dbpath, jspath, options.dbname, dbadd);
				}
				else if (options.action == Options::ActionModify)
				{
					Amd obj;
					obj.update(dbpath, jspath, options.dbname, dbmodify);
				}
				else if (options.action == Options::ActionDelete)
				{
					Amd obj;
					obj.update(dbpath, jspath, options.dbname, dbdelete);
				}
			}
			else
			{
				Options option;
				option.jspath = jspath;
				option.dbname = options.dbname;
				option.action = options.action;
				lstOption.append(option);
			}
		}
	}
	return lstOption;
}

void modifyMultiGSP()
{
	QList<Options> lstOption = modifySingleGSP(false);

	// !!! 规则库路径，这里填备份的路径，会直接修改该路径文件
	QString strFilesDir = QString::fromLocal8Bit("F:/Files");
	QStringList oFileList;
	QStringList oNameFilter;
	oNameFilter << "";
	getFiles(strFilesDir, oNameFilter, oFileList, true);

	int maximum = oFileList.count();
	for (int i = 0; i < maximum; ++i)
	{
		QString strFileName = oFileList.at(i);
		QString sExtractFile = strFilesDir + '/' + strFileName;
		if (sExtractFile.endsWith("CalcRulePermission.gip", Qt::CaseInsensitive))
		{
			continue;
		}
		if (sameStr(extractFileExt(strFileName), ".gip"))
		{
			handleGIPFile(sExtractFile, lstOption);
		}
		else
		{
			continue;
		}
	}
}
