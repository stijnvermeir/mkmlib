#pragma once

#include <mkm/product.h>
#include <mkm/wantslist.h>
#include <mkm/want.h>

#include <QObject>
#include <QtNetwork>
#include <QString>
#include <QVector>

namespace mkm {

class Mkm : public QObject
{
	Q_OBJECT
public:
	Mkm(const QString& endpoint, const QString& appToken, const QString& appSecret, const QString& accessToken, const QString& accessTokenSecret, QObject* parent = 0);

	QNetworkAccessManager& networkAccessManager();

	QVector<mkm::Product> findProduct(const QString& name, int gameId = 1, int languageId = 1, bool isExact = true);
	QNetworkReply* findProductAsync(const QString& name, int gameId = 1, int languageId = 1, bool isExact = true);

	QVector<mkm::Wantslist> getWantsLists();
	QNetworkReply* getWantsListsAsync();

	QVector<mkm::Wantslist> addWantsList(const QString& name, int gameId = 1);
	QNetworkReply* addWantsListAsync(const QString& name, int gameId = 1);

	void addWants(const int wantslistId, const QVector<mkm::Want>& wants);
	QNetworkReply* addWantsAsync(const int wantslistId, const QVector<mkm::Want>& wants);

private:
	void addAuthenticationHeader(const QString& method, QNetworkRequest& request);
	QNetworkReply* get(const QString& url);
	QNetworkReply* post(const QString& url, const QByteArray& data);
	QNetworkReply* put(const QString& url, const QByteArray& data);
	QByteArray waitForIt(QNetworkReply* reply);

	QString endpoint_;
	QString appToken_;
	QString appSecret_;
	QString accessToken_;
	QString accessTokenSecret_;
	QNetworkAccessManager manager_;
};

} // namespace mkm

