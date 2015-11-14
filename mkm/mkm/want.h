#pragma once

#include <QVariant>

namespace mkm {

struct Want
{
	enum Condition
	{
		Mint,
		NearMint,
		Excellent,
		Good,
		LightPlayed,
		Played,
		Poor
	};

	int productId; // 0 when metaproduct
	int metaProductId; // 0 when product
	int amount;
	QVector<int> languageIds;
	Condition minCondition;
	double buyPrice;

	Want()
		: productId(0)
		, metaProductId(0)
		, amount(1)
		, languageIds()
		, minCondition(Excellent)
		, buyPrice(0.0)
	{
	}
};

} // namespace mkm
