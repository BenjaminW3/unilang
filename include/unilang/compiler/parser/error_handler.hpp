#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace unilang
{
	//#########################################################################
	// ! The error handler.
	//#########################################################################
	template <typename BaseIterator, typename LexerIterator>
	struct error_handler
	{
		template <typename, typename, typename>
		struct result {typedef void type;};
		
		//-------------------------------------------------------------------------
		//! Constructor.
		//-------------------------------------------------------------------------
		error_handler(BaseIterator first, BaseIterator last)
		  : first(first), last(last) 
		{
		}
		
		//-------------------------------------------------------------------------
		//! Prints out the error Message, the error reason and the position the error occured.
		//-------------------------------------------------------------------------
		template <typename Message, typename What>
		void operator()(	Message const & message,
							What const & what,
							LexerIterator err_pos) const
		{
			// Retrieve underlying iterator from current token, err_pos points to the last validly matched token, so we use its end iterator as the error position
			BaseIterator err_pos_base (err_pos->matched().end());

			unsigned int uiLine;
			BaseIterator line_start (get_pos(err_pos_base, uiLine));
			if (err_pos_base != last)
			{
				std::cout << message << " " << what << " line " << uiLine << ':' << std::endl;
				std::cout << get_line(line_start) << std::endl;
				for (; line_start != err_pos_base; ++line_start)
				{
					std::cout << ' ';
				}
				std::cout << '^' << std::endl;
			}
			else
			{
				std::cout << "Unexpected end of file. ";
				std::cout << message << what << " line " << uiLine << std::endl;
			}
		}
		
		//-------------------------------------------------------------------------
		//! 
		//-------------------------------------------------------------------------
		BaseIterator get_pos(BaseIterator err_pos, unsigned int & uiLine) const
		{
			uiLine = 1;
			BaseIterator i (first);
			BaseIterator line_start (first);
			while (i != err_pos)
			{
				bool eol (false);
				if (i != err_pos && *i == '\r') // CR
				{
					eol = true;
					line_start = ++i;
				}
				if (i != err_pos && *i == '\n') // LF
				{
					eol = true;
					line_start = ++i;
				}
				if (eol)
					++uiLine;
				else
					++i;
			}
			return line_start;
		}
		
		//-------------------------------------------------------------------------
		//! Gets the line the error occured
		//-------------------------------------------------------------------------
		std::string get_line(BaseIterator err_pos) const
		{
			BaseIterator i (err_pos);
			// position i to the next EOL
			while (i != last && (*i != '\r' && *i != '\n'))
				++i;
			return std::string(err_pos, i);
		}

		BaseIterator first;
		BaseIterator last;
		std::vector<LexerIterator> iters;
	};
}