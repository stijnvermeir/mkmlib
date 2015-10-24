#include "mkm/mkm.h"

#include <QtNetwork>
#include <QMessageAuthenticationCode>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

using namespace mkm;
using namespace std;

namespace {

const QString ENDPOINT = "https://sandbox.mkmapi.eu/ws/v1.1/output.json/";

}

struct Mkm::Pimpl
{
	QString appToken_;
	QString appSecret_;
	QString accessToken_;
	QString accessTokenSecret_;
	QNetworkAccessManager manager_;

	Pimpl(const QString& appToken, const QString& appSecret, const QString& accessToken, const QString& accessTokenSecret)
		: appToken_(appToken)
		, appSecret_(appSecret)
		, accessToken_(accessToken)
		, accessTokenSecret_(accessTokenSecret)
		, manager_()
	{
		qsrand(QDateTime::currentDateTime().toTime_t());
	}

	void addAuthenticationHeader(const QString& method, QNetworkRequest& request)
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

	QByteArray get(const QString& url)
	{
		QNetworkRequest request;
		request.setUrl(ENDPOINT + url);
		addAuthenticationHeader("GET", request);
		QScopedPointer<QNetworkReply> reply(manager_.get(request));
		QEventLoop loop;
		QObject::connect(reply.data(), SIGNAL(finished()), &loop, SLOT(quit()));
		loop.exec();
		if (reply->error())
		{
			qDebug() << reply->error() << reply->errorString();
		}
		return reply->readAll();
	}

	QVector<Product> findProduct(const QString& name, int gameId, int languageId, bool isExact)
	{
		QString url;
		QTextStream str(&url);
		str << "products/" << name << "/" << gameId << "/" << languageId << "/" << (isExact ? "true" : "false");
		auto rsp = get(url);
		auto json  = QJsonDocument::fromJson(rsp);
		// qDebug() << json;
		QVector<Product> products;
		for (const QJsonValue& pv : json.object()["product"].toArray())
		{
			auto p = pv.toObject();
			Product product;
			product.idProduct = p["idProduct"].toInt();
			product.idMetaproduct = p["idMetaproduct"].toInt();
			product.countReprints = p["countReprints"].toInt();
			for (const QJsonValue& nv : p["name"].toObject())
			{
				auto n = nv.toObject();
				int idLanguage = n["idLanguage"].toInt();
				product.names[idLanguage].idLanguage = idLanguage;
				product.names[idLanguage].languageName = n["languageName"].toString();
				product.names[idLanguage].productName = n["productName"].toString();
			}
			auto c = p["category"].toObject();
			product.category.idCategory = c["idCategory"].toInt();
			product.category.categoryName = c["categoryName"].toString();
			auto pg = p["priceGuide"].toObject();
			product.priceGuide.sell = pg["SELL"].toDouble();
			product.priceGuide.low = pg["LOW"].toDouble();
			product.priceGuide.lowExPlus = pg["LOWEX"].toDouble();
			product.priceGuide.lowFoil = pg["LOWFOIL"].toDouble();
			product.priceGuide.avg = pg["AVG"].toDouble();
			product.priceGuide.trend = pg["TREND"].toDouble();
			product.website = p["website"].toString();
			product.image = p["image"].toString();
			product.expansion = p["expansion"].toString();
			product.expIcon = p["expIcon"].toInt();
			product.rarity = p["rarity"].toString();
			for (const QJsonValue& rpv : p["reprint"].toArray())
			{
				auto rp = rpv.toObject();
				Product::Reprint reprint;
				reprint.idProduct = rp["idProduct"].toInt();
				reprint.expansion = rp["expansion"].toString();
				reprint.expIcon = rp["expIcon"].toInt();
				product.reprints.push_back(reprint);
			}
			product.countArticles = p["countArticles"].toInt();
			product.countFoils = p["countFoils"].toInt();
			products.push_back(product);
		}

		return products;
	}
};

Mkm::Mkm(const QString& appToken, const QString& appSecret, const QString& accessToken, const QString& accessTokenSecret)
	: pimpl_(new Pimpl(appToken, appSecret, accessToken, accessTokenSecret))
{
}

Mkm::~Mkm()
{
}

QVector<Product> Mkm::findProduct(const QString& name, int gameId, int languageId, bool isExact)
{
	return pimpl_->findProduct(name, gameId, languageId, isExact);
}
