#pragma once
#include "../../abstract_syntax_tree/abstract_syntax_tree.hpp"

#include "../error_handler.hpp"
#include "../skipper.hpp"
#include "../annotation.hpp"

#include "../spirit.hpp"

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;

		///////////////////////////////////////////////////////////////////////////////
		//  The function grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct identifier_grammar : boost::spirit::qi::grammar<Iterator, ast::identifier(), skipper<Iterator> >
		{
			identifier_grammar(error_handler<Iterator>& error_handler)
			 : identifier_grammar::base_type(identifier)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::raw_type raw;
				qi::lexeme_type lexeme;
				qi::alpha_type alpha;
				qi::alnum_type alnum;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;
				
				/*typenames.add
					("int")
					("uint")
					;*/

				keywords.add
					("true")
					("false")
					("if")
					("else")
					("while")
					("void")
					("return")
					;

				// a identifier begins with a character followed by charcters or numbers
				identifier =
						!keywords
					>>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
					;
				// The directive qi::raw[] reintroduces transduction parsing into Qi. It is doing that by exposing as its attribute the pair of iterators to the input sequence matched by its embedded parser. 
				// To be concise, it exposes an instance  of a boost::iterator_range<Iterator> containing the two iterators (where Iterator is the type of the underlying input stream as passed to qi::parse).
				// http://boost-spirit.com/home/2010/01/14/why-might-i-want-to-use-the-directive-qiraw/

				// The lexeme[] directive turns off white space skipping. http://www.boost.org/doc/libs/1_49_0/libs/spirit/doc/html/spirit/qi/reference/directive/lexeme.html
				identifier.name("identifier");

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(identifier)
				);

				// Error handling: on error in start, call error_handler.
				on_error<fail>(identifier,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in start, call annotation.
				/*on_success(identifier,
					annotation_function(error_handler.iters)(_val, _1));*/
			}

			qi::rule<Iterator, ast::identifier(), skipper<Iterator> > identifier;

			qi::symbols<char> keywords;
		};
	}
}