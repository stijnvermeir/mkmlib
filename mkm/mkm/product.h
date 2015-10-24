#pragma once

#include <QString>
#include <QVector>
#include <QMap>

namespace mkm {

struct Product
{
	struct Name
	{
		int idLanguage;
		QString languageName;
		QString productName;
	};

	struct Category
	{
		int idCategory;
		QString categoryName;
	};

	struct PriceGuide
	{
		double sell;
		double low;
		double lowExPlus;
		double lowFoil;
		double avg;
		double trend;
	};

	struct Reprint
	{
		int idProduct;
		QString expansion;
		int expIcon;
	};

	int idProduct;
	int idMetaproduct;
	int countReprints;
	QMap<int, Name> names;
	Category category;
	PriceGuide priceGuide;
	QString website;
	QString image;
	QString expansion;
	int expIcon;
	int number;
	QString rarity;
	QVector<Reprint> reprints;
	int countArticles;
	int countFoils;
};

} // namespace mkm
