#pragma once

#include <mkm/product.h>

#include <QScopedPointer>
#include <QString>
#include <QVector>

namespace mkm {

class Mkm
{
public:
	Mkm(const QString& appToken, const QString& appSecret, const QString& accessToken, const QString& accessTokenSecret);
	~Mkm();

	QVector<Product> findProduct(const QString& name, int gameId = 1, int languageId = 1, bool isExact = false);
private:
	struct Pimpl;
	QScopedPointer<Pimpl> pimpl_;
};

} // namespace mkm

