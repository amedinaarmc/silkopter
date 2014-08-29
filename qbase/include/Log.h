#pragma once

namespace q
{

#if !defined Q_AVR

namespace logging
{
	class Logger;
	DECLARE_CLASS_PTR(Logger);

/////////////////////////////////////////////////////////////////////////////////////
// Loggers

	//This adds a new logger to the system.
	extern void add_logger(std::unique_ptr<Logger> logger);

	//The logger becomes the only logger. All the others are removed (and deleted)
	extern void set_logger(std::unique_ptr<Logger> logger);

/////////////////////////////////////////////////////////////////////////////////////
//Logging topic
// 	Topics are used to separate between various parts of the game that generate logging info.
//	A game will game a defautl topic and several user topics like 'online', 'multiplayer', 'engine', 'resources' etc


	//This function turns on/off various topics
	extern void set_topic_enabled(String const& topic, bool enabled);

/////////////////////////////////////////////////////////////////////////////////////
// Decoration
//	By default, the decoration is TOPIC | LEVEL | LOCATION | TIME
// 	You can specify the decoration per topic. If none is defined, the default one is used
//	All decorations appear in square paranthesis []. The order is of the decorations is the one in the enum: LEVEL, TOPIC, LOCATION, DATE, TIME

	enum class Decoration : uint8_t
	{
		LEVEL		= 1 << 0,	//the level of the log
		TOPIC		= 1 << 1,	//the topic of the log.
		LOCATION 	= 1 << 2,	//the [file:line] where the log occured
		DATE 		= 1 << 3,	//the date in [day-month-year] format
		TIME		= 1 << 4	//the time in [HH:mm:ss] format
	};
	typedef q::util::Flag_Set<Decoration, uint8_t> Decorations;

	extern void set_decorations(Decorations decorations);
	extern void set_decorations(String const& topic, Decorations decorations);


/////////////////////////////////////////////////////////////////////////////////////
// Log Level - used to specify the importance of the message
//	You can specify the level per topic. 
//	The log level used is defined as max(defaultLevel, topicLevel)

    enum class Level : uint8_t
	{
		DBG,		//used to debug info. Disabled in Release
		INFO,		//used to print usefull info both in Debug and Release
		WARNING,	//used to print warnings that will not crash, both Debug and Release
		ERR			//used for messages that will probably crash or seriously affect the game. Both Debug and Release
	};


	//Logs smaller than the level are not sent to loggers.
	//SetLevel(Level::INFO) will print info, warning and critical logs but not debug
	extern void set_level(Level level);
	extern void set_level(String const& topic, Level level);


/////////////////////////////////////////////////////////////////////////////////////
// The log functions
// Note: Various prototypes are provided to avoid memory allocations as much as possible
// 		 Most of the time the topic is a char const* 

}

	extern void log(logging::Level level, const String& topic, const char* file, int line, const String& message);

	template<class Fmt, typename... Params>
    void logf(logging::Level level, String const& topic, char const* file, int line, Fmt const& fmt, Params&&... params)
	{
		String message;
        q::util::format(message, fmt, std::forward<Params>(params)...);
		log(level, topic, file, line, message);
	}

	template<class Fmt, typename... Params>
    void quick_logf(Fmt const& fmt, Params&&... params)
	{
		String message;
        q::util::format(message, fmt, std::forward<Params>(params)...);
        log(logging::Level::INFO, "", nullptr, 0, message);
	}

#   define QLOG_DBG(topic, fmt, ...) 	q::logf(q::logging::Level::DBG, topic, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#   define QLOG_INFO(topic, fmt, ...) 		q::logf(q::logging::Level::INFO, topic, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#   define QLOG_WARNING(topic, fmt, ...) 	q::logf(q::logging::Level::WARNING, topic, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#   define QLOG_ERR(topic, fmt, ...) 	q::logf(q::logging::Level::ERR, topic, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#else

	template<class Fmt, typename... Params>
    void quick_logf(Fmt const& fmt, Params&&... params)
	{
		if (stdout)
		{
            fputc('\n', stdout);
            q::util::format(*stdout, fmt, params...);
		}
	}

#   define QLOG_DBG(topic, fmt, ...) 	q::quick_logf(fmt, ##__VA_ARGS__)
#   define QLOG_INFO(topic, fmt, ...) 	q::quick_logf(fmt, ##__VA_ARGS__)
#   define QLOG_WARNING(topic, fmt, ...) q::quick_logf(fmt, ##__VA_ARGS__)
#   define QLOG_ERR(topic, fmt, ...) 	q::quick_logf(fmt, ##__VA_ARGS__)

#endif

}

