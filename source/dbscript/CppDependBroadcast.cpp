#include "CppDependBroadcast.h"

//��΢�ŷ���markdown��ʽ��Ϣ
void sendToWeChatWithMarkdown(const QString& api_path, const QString& content)
{
	QNetworkRequest request(api_path);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	QUrlQuery post_data;
	post_data.addQueryItem("msgtype", "markdown");

	QNetworkAccessManager manager;

	QEventLoop loop;
	bool success = QObject::connect(&manager, SIGNAL(finished(QNetworkReply *)),
		&loop, SLOT(quit()));

	QNetworkReply* reply = manager.post(request, post_data.toString().toUtf8());
	loop.exec();

	int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	QByteArray json = reply->readAll();

	reply->deleteLater();
}

//����markdown��ʽ��֪ͨ
QString generateEmail(int added, const QHash<QString, int>& authors, const QString& reportlink)
{
	QString emailBody = QStringLiteral("**<font size=15>�ܹ�����ÿ�ղ�����</font>**\n");
	if (added == 0)
	{
		emailBody += QStringLiteral("Ŀǰû�мܹ������������⣡");
		return emailBody;
	}
	emailBody += QStringLiteral("�ܹ�����Ŀǰ������<font color=\"warning\">") + QString::number(added) +
		QStringLiteral("��</font>���⣬�����Ա���ע����ʱ�޸ģ�\n");
	for (auto iter = authors.begin(); iter != authors.end(); iter++)
	{
		QString author = iter.key();
		int cnt = iter.value();
		emailBody += ">" + author + " : <font color=\"comment\">" + QString::number(cnt) + QStringLiteral("��</font>��������\n");
	}
	emailBody += QStringLiteral("[���Ҳ鿴����](") + reportlink + ")";
	return emailBody;
}

QPair<QJsonDocument, QString> getJsonAndProductData(const QString& html)
{
	QString cpdData;
	//html.find(type="text/babel").string.split("\n"). ֱ�ӽ�html.find(type="text/babel").string��Ϊhtml��������
	const QStringList strings = html.split("\n");
	//QList<QByteArray> strings = QByteArray::split(char sep);
	for (QString AString : strings)
	{
		if (AString.contains("allCPDData: "))
		{
			cpdData = AString;
			break;
		}
	}
	int start = cpdData.indexOf("{");
	int end = cpdData.lastIndexOf("}");
	cpdData = cpdData.mid(start - 1, end - start + 2);

	QJsonParseError error;
	QJsonDocument jsonDoc = QJsonDocument::fromJson(cpdData.toUtf8(), &error);

	QString product;
	for (QString BString : strings)
	{
		if (BString.contains("productName"))
		{
			product = BString;
			break;
		}
	}
	int start1 = product.indexOf("\"");
	int end1 = product.lastIndexOf("\"");
	QString productName = product.mid(start1 + 1, end1 - start1 - 1);
	return qMakePair(jsonDoc, productName);
}

//�������˺�ת��
void transformAccount(QHash<QString, int>& authors, const QString& path)
{
	QString iniPath = path + "Account.ini";
	if (!QFileInfo::exists(iniPath))
	{
		qDebug() << QStringLiteral("�������˺������ļ�ʧ�ܣ�");
		return;
	}
	QSettings config(iniPath, QSettings::IniFormat);
	config.beginGroup("Account");
	SCOPE_EXIT
	{
		config.endGroup();
	};

	QHash<QString, QString> acctDict;
	qDebug() << QStringLiteral("�������˺������ļ��ɹ�");
	const QStringList accounts = config.allKeys();
	for (QString user : accounts)
	{
		QString employeeName = config.value(user).toString();
		acctDict[user] = employeeName;
	}

	QList<QString> keys = authors.keys();
	for (QString author : keys)
	{
		if (!author.contains("100"))
		{
			continue;
		}
		QString JobNo = author.right(8); // TODO JobNo = author[-9:-1]
		if (acctDict.contains(JobNo))
		{
			QString employeeName = acctDict[JobNo];
			authors[employeeName] = authors[author];
			authors.remove(author);
		}
	}
}

GetDataFromHtml::GetDataFromHtml(const QString& html, const QString& db)
{
	QString content;// TODO BeautifulSoup(open(html, 'r', encoding='utf-8'))
	QPair<QJsonDocument, QString> tmpPair = getJsonAndProductData(content);
	m_json = tmpPair.first.object();
	int nCnt = m_json.count();
	m_product = tmpPair.second;

	if (QFileInfo::exists(db))
	{
		m_db = QSqlDatabase::addDatabase("QSQLITE");
		m_db.setDatabaseName(db);
		// �����һ�����ݿ���������ʱ��QSqlDatabase���������ڻ��Զ�����close()���ͷ����ݿ�����
		if (m_db.open())
		{
			QStringList tables = m_db.tables();
			tables.clear();
		}
	}
}

QPair<int, QHash<QString, int>> GetDataFromHtml::getDatas()
{
	int added = 0;
	QHash<QString, int> authors;

	QJsonValue rulesVal = this->m_json["rules"];
	bool isArray = rulesVal.isArray();
	const QJsonArray rulesArr = rulesVal.toArray();
	int nRulesCnt = rulesArr.count();

	for (QJsonValue rule : rulesArr)
	{
		const QJsonObject ruleObj = rule.toObject();
		int nTmp = ruleObj["Added"].toInt();
		added += nTmp;
	}

	for (auto iter1 = this->m_json.begin(); iter1 != this->m_json.end(); iter1++)
	{
		QString key = iter1.key();
		if (key == "measure time" || key == "rules")
		{
			continue;
		}

		QJsonValue subJsonVal = iter1.value();
		isArray = subJsonVal.isArray();
		const QJsonArray subJsonArr = subJsonVal.toArray();
		for (QJsonValue subSubJsonVal : subJsonArr)
		{
			const QJsonObject item = subSubJsonVal.toObject();

			QString sName = item["Full Name"].toString();
			this->m_namelist.append(sName);

			QString author = item["Author"].toString();
			if (author.isEmpty())
			{
				author = "δ֪����";
			}
			if (authors.contains(author))
			{
				authors[author] = authors[author] + 1;
			}
			else
			{
				authors[author] = 1;
			}
		}
	}
	return qMakePair(added, authors);
}

QStringList GetDataFromHtml::getIgnoreDatasFromDB()
{
	QStringList ignoreAuthors;
	if (!this->m_db.isValid())
	{
		qDebug() << QStringLiteral("cpd.db���ݿⲻ���ڣ������4������");
		return ignoreAuthors;
	}

	QString queryStr = QString("SELECT type, name FROM cpditem WHERE product = \"%1\"").arg("GQI");// TODO  this->m_product
	QSqlQuery query = this->m_db.exec(queryStr);
	while (query.next()) {
		QString type = query.value(0).toString();
		QString name = query.value(1).toString();
		if (!this->m_namelist.contains(name))
		{
			continue;
		}
		QPair<QString, int> tmpPair = this->findIgnoreAuthor(type, name);
		QString author = tmpPair.first;
		int count = tmpPair.second;
		if (author.isEmpty())
		{
			author = QStringLiteral("δ֪����");
		}
		for (int i = 0; i < count; i++)
		{
			ignoreAuthors.append(author);
		}
	}
	return ignoreAuthors;
}

QPair<QString, int> GetDataFromHtml::findIgnoreAuthor(const QString& type, const QString& name)
{
	QString author;
	int count = 2;
	// TODO
	return qMakePair(author, count);
}
