#include "GTJRunnable.h"

// ����ȫ�ֱ��������ڷ����쳣ʱ��λ�ļ�
QString g_strGSPFile = QString();

QList<Options> modifySingleGSP(bool bDirectModify);


void handleGIPFile(const QString& sExtractFile, const QList<Options>& lstOption)
{
	// ��ѹ·��
	QString sDestFolder = QCoreApplication::applicationDirPath();
	sDestFolder += "/TempFolder/" + QFileInfo(sExtractFile).baseName() + "/";

	// ������ѹĿ¼
	createDir(sDestFolder);
	QStringList pFiles;
	bool bResult = GMCZipFile::extractFiles(sExtractFile, sDestFolder, &pFiles);//��ѹ
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

		deleteFile(sExtractFile);//ɾ��Դ�ļ�
		//ֱ��ѹ��pFiles�ᵼ��û���ļ���,���Զ���oFiles,����oFiles�д���MDB�ļ���
		oFiles << sDestFolder + "MDB";
		bResult = GMCZipFile::compressFiles(oFiles, QString(), sExtractFile);//ѹ��
	}

	// ɾ����ѹĿ¼
	//deleteTree(sDestFolder);
}

// �ο���windeployqt�������н���˼·
bool parseArguments(const QStringList &arguments,
	Options& options)
{
	QCommandLineParser parser;
	// �������ȳ���һ���ַ���ѡ���dbname
	parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

	QCommandLineOption dbnameOption(QStringLiteral("dbname"),
		QStringLiteral("���ݿ����ƣ���Business��BQCalcRule"),
		QStringLiteral("databaseName"));
	parser.addOption(dbnameOption);

	QCommandLineOption actionOption(QStringLiteral("a"),
		QStringLiteral("ִ�еĲ���"),
		QStringLiteral("action"));
	parser.addOption(actionOption);

	QCommandLineOption dbpathOption(QStringLiteral("d"),
		QStringLiteral("���ݿ�"),
		QStringLiteral("database"));
	parser.addOption(dbpathOption);

	QCommandLineOption jspathOption(QStringLiteral("j"),
		QStringLiteral("�޸���"),
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

QList<Options> modifySingleGSP(bool bSingleGSP) // Ĭ��Ϊtrue
{
	QList<Options> lstOption;
	QRegularExpression sep("\\s+");

	// !!! bat�ű�·��
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
			if (line.startsWith("::")) // ����ע����
			{
				continue;
			}

			if (line.startsWith("set ")) // set�ӿո񣬱����˰���set�ĵ���
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

	// !!! �����·����������ݵ�·������ֱ���޸ĸ�·���ļ�
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
