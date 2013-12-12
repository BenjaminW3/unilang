#pragma once

#define LOG_TYPE output_to_clog
#define LOG_PATH "log"
#define LOG_INDENT_SCOPES

#include <boost/utility.hpp>

// std-Types
#include <sstream>			//std::ostringstream
#include <fstream>			//std::ofstream
#include <mutex>
#include <string>

//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------
#define LOG(output)				(unilang::logging::log() << unilang::logging::log_level::quiet << output)

#define LOG_FUNC(output)		(unilang::logging::log() << unilang::logging::log_level::quiet << __FUNCSIG__ << " " << output)

#define LOG_SCOPE				LOG_SCOPE_FUNCT(unilang::logging::log_level::quiet)

#define LOG_SCOPE_FUNCT(level)	unilang::logging::detail::ScopeLogger s##__LINE____FUNCTION__(level, __FUNCSIG__)

// What we could need to debug
#ifdef RW_DIAGNOSTIC_DEBUG
	#define LOG_FUNC_DIAGNOSTIC(output)	unilang::logging::log()	<<	unilang::logging::log_level::all	<<	__FUNCSIG__	<<	" "	<<	output;
	#define LOG_SCOPE_DIAGNOSTIC		LOG_SCOPE_FUNCT(unilang::logging::log_level::all)
#else
	#define LOG_FUNC_DIAGNOSTIC(output)
	#define LOG_SCOPE_DIAGNOSTIC
#endif

// What we need to debug
#if (defined RW_ADDITIONAL_DEBUG) || (defined RW_DIAGNOSTIC_DEBUG)
	#define LOG_FUNC_ADDITIONAL(output)	unilang::logging::log()	<<	unilang::logging::log_level::additional	<<	__FUNCSIG__	<<	" "	<<	output;
	#define LOG_SCOPE_ADDITIONAL		LOG_SCOPE_FUNCT(rw::logging::log_level::additional)
#else
	#define LOG_FUNC_ADDITIONAL(f)
	#define LOG_SCOPE_ADDITIONAL
#endif

// What users could need on their PC`s
#if (defined _DEBUG) || (defined RW_ADDITIONAL_DEBUG) || (defined RW_DIAGNOSTIC_DEBUG)
	#define LOG_FUNC_DEBUG(output)		unilang::logging::log()	<<	unilang::logging::log_level::info	<<	__FUNCSIG__	<<	" "	<<	output;
	#define LOG_SCOPE_DEBUG			LOG_SCOPE_FUNCT(unilang::logging::log_level::info)
#else
	#define LOG_FUNC_DEBUG(f)
	#define LOG_SCOPE_DEBUG
#endif

#define LOG_FUNC_WARNING(output)			unilang::logging::log()	<<	unilang::logging::log_level::warning	<<	__FUNCSIG__	<<	" "	<<	output;
#define LOG_FUNC_WARNING_ONCE(output)		{static bool rw_bWarningOnce = false; if(!rw_bWarningOnce){LOG_FUNC_WARNING(output);rw_bWarningOnce=true;}}

//#define LOG_FUNC_ERROR(output)			unilang::logging::log()	<<	unilang::logging::log_level::error	<<	__FUNCSIG__	<<	" "	<<	output;	DEBUG_BREAK;

// A run-time assertion. In debug builds, violating an assertion stops execution and informs the debugger.
//#define RW_ASSERT_THROW(_expression) if(!(_expression)) {DEBUG_BREAK; throw std::exception(EXCEPTION_STRING(std::wstring(#_expression)+" Failed"));}
#define RW_ASSERT_ERROR(_expression) if(!(_expression)) {LOG_FUNC_ERROR(std::string(#_expression)+L" Failed");}

namespace unilang
{
	//-----------------------------------------------------------------------------
	//! The logging infrastructure.
	//! 
	//! The no_output policy is stricly optional it can be used to disable logging.
	//-----------------------------------------------------------------------------
	namespace logging
	{
		//-----------------------------------------------------------------------------
		//! Controls the log output.
		//-----------------------------------------------------------------------------
		enum log_level
		{
			quiet,    // no output
			error,    // only errors are output
			warning,  // errors and warnings are output
			info,     // errors, warnings, and informative messages
			additional,// errors, warnings, and informative messages
			all,      // all messages are output
			none,     // end of enum marker (none level is set at all)
		};
		
		//-----------------------------------------------------------------------------
		//! Controls the indentation of the log output.
		//-----------------------------------------------------------------------------
		enum class indentation : short
		{
			indent = 1,
			outdent = -1,
		};

		//-----------------------------------------------------------------------------
		//! Functions for detail implementations of logging.
		//-----------------------------------------------------------------------------
		namespace detail
		{
			//-----------------------------------------------------------------------------
			//! \return The formatted date
			//-----------------------------------------------------------------------------
			std::string dateTime();
			//-----------------------------------------------------------------------------
			//! gets the date in the form specified by m_eLogType
			//!
			//! if LOG_DATETIME is defined, the Date string will be printed before each message
			//! if LOG_MS is defined, the time difference between this and the last log call is printed
			//-----------------------------------------------------------------------------
			std::string GetDateTimeString();

			//-----------------------------------------------------------------------------
			//! Generates the LogLevel line beginning
			//-----------------------------------------------------------------------------
			std::string GenerateLogLevelString();

			//#############################################################################
			//! Used To log the Creation inside the Scope until leaving of the scope.
			//#############################################################################
			class ScopeLogger sealed : boost::noncopyable
			{
			public:
				ScopeLogger( log_level const & level, std::string const & sFct );
				~ScopeLogger();

			private:
				log_level const _level;
				std::string const _sFct;
			};
			//#############################################################################
			//! interface forwarding the desired logging policy to the log class
			//! the << calls of one command line are saved to a stream buffer.
			//! at destruction the collected messages are send to the desired output
			//#############################################################################
			template<class OutputPolicy>
			class logstream abstract :	public std::ostringstream,
										boost::noncopyable
			{
			public:
				//-----------------------------------------------------------------------------
				//! Constructor.
				//-----------------------------------------------------------------------------
				logstream()
					:m_log_level(all),	// TODO: make changeable
					m_current_level(none)
				{
				}
				//-----------------------------------------------------------------------------
				//! Destructor.
				//-----------------------------------------------------------------------------
				~logstream()
				{
					if(!str().empty())
					{
						flush();
						OutputPolicy()(*this);
					}
				}

				//-----------------------------------------------------------------------------
				//! If level is high enough, writes buffer to the log file.
				//! Clears the buffer.
				//-----------------------------------------------------------------------------
				/*void flush()
				{
					m_current_level=none;
				}*/

				//-----------------------------------------------------------------------------
				//! inherits EVERYTHING that's not explicitely overloaded in this class
				//-----------------------------------------------------------------------------
				template <typename T>
				inline logstream & operator<<( T const & t )
				{
					if (m_current_level<=m_log_level)
					{
						(*(std::ostringstream*)this) << t;
					}
					return *this;
				}

				// stubs for manipulators
				typedef logstream & (*logstream_manip)(logstream &);
				logstream & operator<<(logstream_manip manip) { return manip(*this); }
				
				//-----------------------------------------------------------------------------
				// log level
				//-----------------------------------------------------------------------------
				inline logstream & operator<<(log_level const & level)
				{
					m_current_level=level; 
					return *this;
				} 

				// sets the acceptable message level until next flush/endl
				inline void set_level(log_level level) { m_current_level = level; }
				inline log_level get_level() const { return m_current_level; }

				inline void change_log_level(log_level level) { m_log_level = level; }
				inline log_level get_log_level() const { return m_log_level; }

				//-----------------------------------------------------------------------------
				// indentation
				//-----------------------------------------------------------------------------
				inline logstream & operator<<(indentation const & ind) 
				{
					if(! ((s_uiIndent==0) && (ind==indentation::outdent)) )
					{
						s_uiIndent += unsigned int(short(ind));
					}
					return *this;
				} 
				std::string get_indent_whitespace() const
				{
					if(s_bAllowIndentation)
					{
						return std::string(s_uiIndent, ' ');
					}
					else
					{
						return "";
					}
				}
				std::string get_indent_count() const
				{
					if(s_bAllowIndentation)
					{
						return s_uiIndent;
					}
					else
					{
						return 0;
					}
				}

			private:
				log_level const m_log_level;
				log_level m_current_level; // only until the next flush/endl

				static bool s_bAllowIndentation;
				static unsigned int s_uiIndent;
			};

			//-----------------------------------------------------------------------------
			//! Setlevel is reset after each line, therefore it applies to the current log line only.
			//-----------------------------------------------------------------------------
			struct __logstream_level { log_level _m_level; };
			inline __logstream_level setlevel(log_level _level)
			{
				__logstream_level level = { _level };
				return level;
			}
			template<typename T>
			inline logstream<T> & operator<<(logstream<T> & out, const __logstream_level & level )
			{
				out.set_level(level._m_level);
				return out;
			}

			//#############################################################################
			//! No logging at all.
			//#############################################################################
			class no_output sealed
			{
			private:
				struct null_buffer
				{
					template<class T>
					null_buffer &operator<<(const T &)
					{
						return *this;
					}
				};
			public:
				typedef null_buffer stream_buffer;

			public:
				//-----------------------------------------------------------------------------
				//! writes nothing
				//-----------------------------------------------------------------------------
				void operator()(const stream_buffer &)
				{
				}
			};

			//#############################################################################
			//! Log to File
			//#############################################################################
			class output_to_file sealed
			{
			private:
				// extend the file
				class log_file : public std::ofstream
				{
				public:
					//-----------------------------------------------------------------------------
					//! creates the log File
					//-----------------------------------------------------------------------------
					log_file();
					~log_file();
				};

				std::mutex & mutex();

			public:
				//-----------------------------------------------------------------------------
				//! writes the stream to the log
				//-----------------------------------------------------------------------------
				void operator()( logstream<output_to_file> const & s );
			};

			//#############################################################################
			//! Log to html.
			//! If there are any of the following strings in the log message, the corresponding lines will be highlighted in special colors:
			//! [ERROR] [WARNING] [DEBUG] [ADDITIONAL] [DIAGNOSTIC] 
			//#############################################################################
			class output_to_html_file sealed
			{
			private:
				// extend the file with html surroundings
				class log_file_html : public std::ofstream
				{
				public:
					//-----------------------------------------------------------------------------
					//! creates the html Log File
					//-----------------------------------------------------------------------------
					log_file_html();
					~log_file_html();
				};

				std::mutex & mutex();

			public:
				//-----------------------------------------------------------------------------
				//! writes the stream to the log
				//-----------------------------------------------------------------------------
				void operator()( logstream<output_to_html_file> const & s );

			private:
				//-----------------------------------------------------------------------------
				//! \return The html color string corresponding to the given log level
				//-----------------------------------------------------------------------------
				std::string GetLogLevelColorString( log_level const & level );
			};

			//#############################################################################
			//! Standard logging to cout.
			//#############################################################################
			class output_to_clog sealed
			{
			public:
				void operator()( logstream<output_to_clog> const & s );

			private:
				std::mutex & mutex();
			};
		}

		//#############################################################################
		//! Log class used to log a message.
		//! logging::log() << "this is a test" << 1234 << "testing";
		//! Note the lack of a '\n' and std::endl since it's implicit. The contents are buffered and then atomically output using the template specified policy.
		//#############################################################################
		class log sealed: public detail::logstream<detail::LOG_TYPE>
		{
		};
	}
}

/*namespace std
{
	//-----------------------------------------------------------------------------
	//! Overloads endl for logstreams.
	//! endl automatically flushes the stream. The Flush resets the current message log level.
	//-----------------------------------------------------------------------------
	inline rw::logging::detail::logstream<rw::logging::detail::LOG_TYPE> & endl(rw::logging::detail::logstream<rw::logging::detail::LOG_TYPE> & out)
	{
		out.put('\n');
		out.flush();
		return out; 
	}
}*/