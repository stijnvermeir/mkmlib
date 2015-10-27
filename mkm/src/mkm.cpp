#include "mkm/mkm.h"
#include "mkm/parser.h"

#include <QMessageAuthenticationCode>
#include <QEventLoop>
#include <QScopedPointer>

using namespace mkm;
using namespace std;

Mkm::Mkm(const QString& endpoint, const QString& appToken, const QString& appSecret, const QString& accessToken, const QString& accessTokenSecret, QObject* parent)
	: QObject(parent)
	, endpoint_(endpoint)
	, appToken_(appToken)
	, appSecret_(appSecret)
	, accessToken_(accessToken)
	, accessTokenSecret_(accessTokenSecret)
	, manager_()
{
}

QNetworkAccessManager& Mkm::networkAccessManager()
{
	return manager_;
}

QVector<mkm::Product> Mkm::findProduct(const QString& name, int gameId, int languageId, bool isExact)
{
	QScopedPointer<QNetworkReply> reply(findProductAsync(name, gameId, languageId, isExact));
	return parseProducts(waitForIt(reply.data()));
}

QNetworkReply* Mkm::findProductAsync(const QString& name, int gameId, int languageId, bool isExact)
{
	QString url;
	QTextStream str(&url);
	str << "products/" << name << "/" << gameId << "/" << languageId << "/" << (isExact ? "true" : "false");
	return get(url);
}

void Mkm::addAuthenticationHeader(const QString& method, QNetworkRequest& request)
{
	QString nonce = QString::number(qrand(), 16) + QString::number(qrand(), 16);
	QString timestamp = QString::number(QDateTime::currentDateTimeUtc().toTime_t());

	QByteArray message;
	message += method + "&";
	message += QUrl::toPercentEncoding(request.url().toEncoded()) + "&";
	message += "oauth_consumer_key%3D" + appToken_ + "%26";
	message += "oauth_nonce%3D" + nonce + "%26";
	message += "oauth_signature_method%3DHMAC-SHA1%26";
	message += "oauth_timestamp%3D" + timestamp + "%26";
	message += "oauth_token%3D" + accessToken_ + "%26";
	message += "oauth_version%3D1.0";

	// qDebug() << "Message:" << message;

	QByteArray key;
	key += appSecret_ + "&" + accessTokenSecret_;

	auto rawSignature = QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha1);
	QString signature(rawSignature.toBase64());

	QByteArray header;
	header += "OAuth ";
	header += "realm=\"" + request.url().toEncoded() + "\", ";
	header += "oauth_consumer_key=\"" + appToken_ + "\", ";
	header += "oauth_token=\"" + accessToken_ + "\", ";
	header += "oauth_nonce=\"" + nonce + "\", ";
	header += "oauth_timestamp=\"" + timestamp + "\", ";
	header += "oauth_signature_method=\"HMAC-SHA1\", ";
	header += "oauth_version=\"1.0\", ";
	header += "oauth_signature=\"" + signature + "\"";

	// qDebug() << "Header:" << header;

	request.setRawHeader("Authorization", header);
}

QNetworkReply* Mkm::get(const QString& url)
{
	QNetworkRequest request;
	request.setUrl(endpoint_ + url);
	addAuthenticationHeader("GET", request);
	return manager_.get(request);
}

QByteArray Mkm::waitForIt(QNetworkReply* reply)
{
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	if (reply->error())
	{
		qDebug() << reply->error() << reply->errorString();
	}
	return reply->readAll();
}
