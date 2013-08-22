#pragma once

#include <sstream>

#include <glm/gtc/type_ptr.hpp>

#include <glow/glow.h>

namespace glow {

/** \brief The LogMessage class encapsulates a simple log message and its severity level.

	LogMessages are handled and dispatched by the global logging handler which has to be a subclass of AbstractLogHandler.

	\see logging.h
	\see AbstractLogHandler
*/
class GLOW_API LogMessage
{
public:
	enum Level
	{
		Fatal,
		Critical,
		Warning,
		Debug,
		Info
	};

	LogMessage(Level level, const std::string& message);

	Level level() const;
	const std::string& message() const;

protected:
	Level m_level;
	std::string m_message;
};

class AbstractLogHandler;

/** \brief The LogMessageBuilder class builds a LogMessage from different kinds of primitive types.

	The LogMessageBuilder is  usually created by one of the global functions log, debug, warning, error or fatal.
	It works similar to streams and accepts a number of different types which will be converted to strings automatically.
	When it goes out of scope, it creates a LogMessage from all streamed objects and sends it to the log handler.

	Typeical usage of the LogMessageBuilder:
	\code{.cpp}
		warning() << "This is warning number " << 3;
	\endcode

	\see logging.h
	\see LogMessage
	\see setLoggingHandler
	\see log
	\see debug
	\see warning
*/
class GLOW_API LogMessageBuilder : public std::stringstream
{
public:
	LogMessageBuilder(LogMessage::Level level, AbstractLogHandler* handler);
	LogMessageBuilder(const LogMessageBuilder& builder);
	virtual ~LogMessageBuilder();

	// primitive types
	LogMessageBuilder & operator<<(const char * c);
    LogMessageBuilder & operator<<(const wchar_t * c);
	LogMessageBuilder & operator<<(bool b);
	LogMessageBuilder & operator<<(char c);
    LogMessageBuilder & operator<<(unsigned char uc);
    LogMessageBuilder & operator<<(short s);
    LogMessageBuilder & operator<<(unsigned short us);
    LogMessageBuilder & operator<<(int i);
    LogMessageBuilder & operator<<(unsigned int ui);
    LogMessageBuilder & operator<<(float f);
	LogMessageBuilder & operator<<(double d);
    LogMessageBuilder & operator<<(long double d);
	LogMessageBuilder & operator<<(long l);
	LogMessageBuilder & operator<<(unsigned long ul);
	LogMessageBuilder & operator<<(void * pointer);
    LogMessageBuilder & operator<<(const std::string & str);
    LogMessageBuilder & operator<<(const std::wstring & str);

	// manipulators
	LogMessageBuilder& operator<<(std::ostream& (*manipulator)(std::ostream&));

	// pointers
	template <typename T>
	LogMessageBuilder& operator<<(T* t_pointer);

	// glm types
	LogMessageBuilder& operator<<(const glm::vec2& v);
	LogMessageBuilder& operator<<(const glm::vec3& v);
	LogMessageBuilder& operator<<(const glm::vec4& v);

	LogMessageBuilder& operator<<(const glm::mat2& m);
	LogMessageBuilder& operator<<(const glm::mat3& m);
	LogMessageBuilder& operator<<(const glm::mat4& m);
protected:
	LogMessage::Level m_level;
	AbstractLogHandler* m_handler;
};

template <typename T>
LogMessageBuilder& LogMessageBuilder::operator<< (T * t_pointer) 
{
    return *this << static_cast<void*>(t_pointer); 
}

} // namespace glow
