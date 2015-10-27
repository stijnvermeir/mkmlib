#pragma once

#include <mkm/product.h>

#include <QVector>
#include <QByteArray>

namespace mkm {

QVector<Product> parseProducts(const QByteArray& data);

}
