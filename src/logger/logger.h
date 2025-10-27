#pragma once

namespace logger
{
	struct logData {};
	struct logError			: logData {};
	struct logWarning		: logData {};
	struct logNotifaction	: logData {};
}