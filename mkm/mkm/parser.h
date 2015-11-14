#pragma once

#include <mkm/product.h>
#include <mkm/wantslist.h>

#include <QVector>
#include <QByteArray>

namespace mkm {

QVector<Product> parseProducts(const QByteArray& data);

QVector<Wantslist> parseWantslists(const QByteArray& data);

void parseWants(const QByteArray& data);

}
