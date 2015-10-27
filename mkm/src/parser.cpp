#include "mkm/parser.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>

using namespace std;
using namespace mkm;

QVector<Product> mkm::parseProducts(const QByteArray& data)
{
	auto json  = QJsonDocument::fromJson(data);
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
