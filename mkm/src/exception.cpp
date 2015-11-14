#include "mkm/exception.h"

using namespace mkm;

MkmException::MkmException(const MkmException::ErrorCode errorCode, const QString& errorMessage, const QString& errorDetails)
	: errorCode_(errorCode)
	, errorMessage_(errorMessage)
	, errorDetails_(errorDetails)
{
}

MkmException::ErrorCode MkmException::getErrorCode() const
{
	return errorCode_;
}

const QString& MkmException::getErrorMessage() const
{
	return errorMessage_;
}

const QString& MkmException::getErrorDetails() const
{
	return errorDetails_;
}

void MkmException::raise() const
{
	throw *this;
}

MkmException* MkmException::clone() const
{
	return new MkmException(*this);
}
