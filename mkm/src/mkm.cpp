#include "mkm/mkm.h"
#include "mkm/parser.h"
#include "mkm/exception.h"

#include <QMessageAuthenticationCode>
#include <QEventLoop>
#include <QScopedPointer>
#include <QXmlStreamWriter>

using namespace mkm;
using namespace std;

namespace {

const QVector<QString> CONDITION_CODE =
{
	"MT",
	"NM",
	"EX",
	"GD",
	"LP",
	"PL",
	"PO"
};

} // namespace

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

QVector<Wantslist> Mkm::getWantsLists()
{
	QScopedPointer<QNetworkReply> reply(getWantsListsAsync());
	return parseWantslists(waitForIt(reply.data()));
}

QNetworkReply* Mkm::getWantsListsAsync()
{
	return get("wantslist");
}

QVector<Wantslist> Mkm::addWantsList(const QString& name, int gameId)
{
	QScopedPointer<QNetworkReply> reply(addWantsListAsync(name, gameId));
	return parseWantslists(waitForIt(reply.data()));
}

QNetworkReply* Mkm::addWantsListAsync(const QString& name, int gameId)
{
	QByteArray data;
	QXmlStreamWriter xml(&data);
	xml.writeStartDocument();
	xml.writeStartElement("request");
	xml.writeStartElement("wantslist");
	xml.writeTextElement("idGame", QString::number(gameId));
	xml.writeTextElement("name", name);
	xml.writeEndElement();
	xml.writeEndElement();
	xml.writeEndDocument();
	return post("wantslist", data);
}

void Mkm::addWants(const int wantslistId, const QVector<Want>& wants)
{
	QScopedPointer<QNetworkReply> reply(addWantsAsync(wantslistId, wants));
	parseWants(waitForIt(reply.data()));
}

QNetworkReply* Mkm::addWantsAsync(const int wantslistId, const QVector<Want>& wants)
{
	QByteArray data;
	QXmlStreamWriter xml(&data);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	xml.writeStartElement("request");
	xml.writeTextElement("action", "add");
	for (const Want& want : wants)
	{
		if (want.metaProductId != 0)
		{
			xml.writeStartElement("metaproduct");
			xml.writeTextElement("idMetaproduct", QString::number(want.metaProductId));
		}
		else
		{
			xml.writeStartElement("product");
			xml.writeTextElement("idProduct", QString::number(want.productId));
		}
		xml.writeTextElement("count", QString::number(want.amount));
		for (int languageId : want.languageIds)
		{
			xml.writeTextElement("idLanguage", QString::number(languageId));
		}
		xml.writeTextElement("minCondition", CONDITION_CODE[want.minCondition]);
		xml.writeTextElement("wishPrice", QString::number(want.buyPrice));
		xml.writeEndElement();
	}
	xml.writeEndElement();
	xml.writeEndDocument();
	// qDebug() << data;
	return put(QString("wantslist/%1").arg(wantslistId), data);
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

QNetworkReply* Mkm::post(const QString& url, const QByteArray& data)
{
	QNetworkRequest request;
	request.setUrl(endpoint_ + url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");
	addAuthenticationHeader("POST", request);
	return manager_.post(request, data);
}

QNetworkReply* Mkm::put(const QString& url, const QByteArray& data)
{
	QNetworkRequest request;
	request.setUrl(endpoint_ + url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");
	addAuthenticationHeader("PUT", request);
	return manager_.put(request, data);
}

QByteArray Mkm::waitForIt(QNetworkReply* reply)
{
	QEventLoop loop;
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	loop.exec();
	if (reply->error())
	{
		MkmException::ErrorCode errorCode = MkmException::ErrorUnknown;
		if (reply->error() == QNetworkReply::AuthenticationRequiredError)
		{
			errorCode = MkmException::ErrorAuthentication;
		}
		throw MkmException(errorCode, reply->errorString(), QString::fromUtf8(reply->readAll()));
	}
	return reply->readAll();
}
