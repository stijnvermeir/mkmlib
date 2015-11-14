#pragma once

#include <QException>
#include <QString>

namespace mkm {

class MkmException : public QException
{
public:
	enum ErrorCode
	{
		ErrorUnknown,
		ErrorAuthentication,
		ErrorParser
	};

	MkmException(const ErrorCode errorCode = ErrorUnknown, const QString& errorMessage = QString(), const QString& errorDetails = QString());

	ErrorCode getErrorCode() const;
	const QString& getErrorMessage() const;
	const QString& getErrorDetails() const;

	void raise() const;
	MkmException* clone() const;

private:
	ErrorCode errorCode_;
	QString errorMessage_;
	QString errorDetails_;
};

} // namespace mkm
