#include <unilang/compiler/log/log.hpp>

#if defined LOG_ALWAYS_OUTPUT_DEBUG_STRING || defined _DEBUG
#	include <windows.h>	// OutputDebugString
#endif

// C++ Standard Library Header Files
#include <ctime>

// std-Types
#include <iostream>				// wclog

namespace unilang
{
	namespace logging
	{
		namespace detail
		{
			template<class OutputPolicy>
			bool logstream<OutputPolicy>::s_bAllowIndentation = 
#ifdef LOG_INDENT_SCOPES
										true
#else
										false
#endif
										;
			template<class OutputPolicy>
			unsigned int logstream<OutputPolicy>::s_uiIndent = 0;
			
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::string dateTime()
			{
				struct tm newtime;
				__time64_t long_time;
				errno_t err;

				// Get time as 64-bit integer.
				_time64( &long_time ); 
				// Convert to local time.
				err = _localtime64_s( &newtime, &long_time ); 
				if (err)
				{
					throw std::runtime_error("Invalid argument to _localtime64_s.");
				}

				char buf[128];
				strftime(buf, 128, "%Y-%m-%d %H:%M:%S", &newtime);
				return buf;
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::string GetDateTimeString()
			{
				long double dNow = std::chrono::duration<long double, std::ratio<1>>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
				static long double dLastTime = dNow;
				std::ostringstream ostrstream;

#ifdef LOG_DATETIME
				ostrstream	<<	dateTime();
#endif
#ifdef LOG_MS
				ostrstream.setf(std::ios::fixed,std::ios::floatfield);		// floatfield set to fixed
				ostrstream.precision(10);

				ostrstream		<<	dNow-dLastTime	<<	"s ";

				dLastTime=dNow;
#endif

				return ostrstream.str();
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::string GetLogLevelString( log_level const & level )
			{
				switch (level)
				{
				case log_level::error:
					return "[ERROR]";
				case log_level::warning:
					return "[WARNING]";
				case log_level::info:
					return "[INFO]";
				case log_level::additional:
					return "[ADDITIONAL]";
				case log_level::all:
					return "[DIAGNOSTIC]";
				default:
					return "";
				}
			}
			
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			ScopeLogger::ScopeLogger( log_level const & level, std::string const & sFct )
			 :	_level(level),
				_sFct(sFct)
			{
				logging::log() << _level << "[+] " << _sFct;
				logging::log() << indentation::indent;
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			ScopeLogger::~ScopeLogger()
			{
				logging::log() << indentation::outdent << _level << "[-] " << _sFct;
			}

			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			output_to_file::log_file::log_file()
				:std::ofstream(LOG_PATH ".txt", std::ios::out)
			{
			}
			output_to_file::log_file::~log_file()
			{
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::mutex & output_to_file::mutex()
			{
				static std::mutex mutex;
				return mutex;
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			void output_to_file::operator()( logstream<output_to_file> const & s )
			{
				std::lock_guard<std::mutex> lock(mutex());

				static log_file file;

				std::ostringstream buffer;
				buffer	<< s.get_indent_whitespace()
						<< GetLogLevelString(s.get_level()) << " "
						<< s.str()	
						<< std::endl;
				std::string bufstr = buffer.str();
#if defined LOG_ALWAYS_OUTPUT_DEBUG_STRING || defined _DEBUG
				OutputDebugStringA(bufstr.c_str());			// http://www.softwareverify.com/blog/?p=261
#endif
				file	<< GetDateTimeString()
						<< bufstr;
				file.flush();
			}

			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			output_to_html_file::log_file_html::log_file_html()
				:std::ofstream(LOG_PATH ".html", std::ios::out)
			{
				*this	<<	L"<html><head><title>Log</title></head><body><p>";
				flush();
			}
			output_to_html_file::log_file_html::~log_file_html()
			{
				*this	<<	L"</p></body></html>";
				flush();
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::mutex & output_to_html_file::mutex()
			{
				static std::mutex mutex;
				return mutex;
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			void output_to_html_file::operator()( logstream<output_to_html_file> const & s )
			{
				std::lock_guard<std::mutex> lock(mutex());

				static log_file_html file;

				std::ostringstream buffer;
				buffer	<< s.get_indent_whitespace()
						<< GetLogLevelString(s.get_level()) << " "
						<< s.str()
						<< std::endl;
				std::string bufstr = buffer.str();
#if defined LOG_ALWAYS_OUTPUT_DEBUG_STRING || defined _DEBUG
				OutputDebugStringA(bufstr.c_str());			// http://www.softwareverify.com/blog/?p=261
#endif		
				file	<< "<span style='font-size: 10px;background-color:"+GetLogLevelColorString(s.get_level())+"'>"
						<< GetDateTimeString()
						<< bufstr
						<< "</span><br />";
				file.flush();
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::string output_to_html_file::GetLogLevelColorString( log_level const & level )
			{
				switch (level)
				{
				case log_level::error:
					return "#FE2E2E";
				case log_level::warning:
					return "#FAAC58";
				case log_level::info:
					return "#F5C040";
				case log_level::additional:
					return "#81F781";
				case log_level::all:
					return "#ACFA58";
				default:
					return "#A9F5F2";
				}
			}

			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			std::mutex & output_to_clog::mutex()
			{
				static std::mutex mutex;
				return mutex;
			}
			//-------------------------------------------------------------------------
			//
			//-------------------------------------------------------------------------
			void output_to_clog::operator()( logstream<output_to_clog> const & s )
			{
				std::lock_guard<std::mutex> lock(mutex());
				std::string sString = GetLogLevelString(s.get_level());

				std::clog
					<< s.get_indent_whitespace()
					;

				if(sString.empty())
				{
					std::clog << sString << " ";
				}
#if defined LOG_DATETIME || LOG_MS
				std::clog
					<< GetDateTimeString() <<	": ";
#endif
				std::clog
					<< s.str()
					<< std::endl;

				std::clog.flush();
			}
		}
	}
}