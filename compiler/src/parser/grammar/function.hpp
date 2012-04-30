#pragma once

#include "statement.hpp"
#include "../error_handler.hpp"
#include "../annotation.hpp"

#include <boost/spirit/include/qi_omit.hpp>
#include <boost/spirit/include/qi_lit.hpp>

namespace unilang
{ 
	namespace parser
	{
		///////////////////////////////////////////////////////////////////////////////
		//  The function grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct function_grammar : qi::grammar<Iterator, ast::function(), skipper<Iterator> >
		{
			function_grammar(error_handler<Iterator>& error_handler)
			 : function_grammar::base_type(function_definition), body(error_handler)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::_val_type _val;
				qi::raw_type raw;
				qi::lexeme_type lexeme;
				qi::alpha_type alpha;
				qi::alnum_type alnum;
				qi::string_type string;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				//boost::spirit::qi::omit_type omit;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				// a type identifier begins with a character followed by charcters or numbers
				type_identifier =
						body.expr.typenames
					//>>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
					;
				type_identifier.name("type_identifier");

				// a variable identifier begins with a character followed by charcters or numbers
				variable_identifier =
						!body.expr.keywords
					>>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
					;
				variable_identifier.name("variable_identifier");

				// a variable identifier begins with a character followed by charcters or numbers
				variable_definition =
						type_identifier
					>	omit[lexeme[' ']]
					>	variable_identifier
					>	'('
					>	')'
					;
				variable_definition.name("variable_definition");

				// a function identifier begins with a character followed by charcters or numbers
				function_identifier =
						!body.expr.keywords
					>>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
					;
				// The directive qi::raw[] reintroduces transduction parsing into Qi. It is doing that by exposing as its attribute the pair of iterators to the input sequence matched by its embedded parser. 
				// To be concise, it exposes an instance  of a boost::iterator_range<Iterator> containing the two iterators (where Iterator is the type of the underlying input stream as passed to qi::parse).
				// http://boost-spirit.com/home/2010/01/14/why-might-i-want-to-use-the-directive-qiraw/

				// The lexeme[] directive turns off white space skipping. http://www.boost.org/doc/libs/1_41_0/libs/spirit/doc/html/spirit/qi/reference/directive/lexeme.html
				function_identifier.name("function_identifier");

				argument_list =
						'('
					>	-( variable_definition % ',')
					>	')'
					;
				argument_list.name("argument_list");

				return_list =
						'('
					>	-( variable_definition % ',')
					>	')'
					;
				return_list.name("return_list");

				function_header =
						function_identifier
					>	argument_list
					>	':'
					>   return_list
					;
				function_header.name("function_header");

				scoped_block =
						'{'
					>	body
					>	'}'
					;
				codeblock.name("codeblock");

				function_definition =
						function_identifier
					>	argument_list
					>	':'
					>   return_list
					>   scoped_block
					;
				function_definition.name("function_definition");

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(variable_identifier)
					(function_identifier)
					(argument_list)
					(return_list)
					(function_definition)
				);

				// Error handling: on error in start, call error_handler.
				on_error<fail>(function_definition,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in start, call annotation.
				/*on_success(identifier,
					annotation_function(error_handler.iters)(_val, _1));*/
			}

			statement_grammar<Iterator> body;
			qi::rule<Iterator, ast::identifier(), skipper<Iterator> > type_identifier;
			qi::rule<Iterator, ast::identifier(), skipper<Iterator> > variable_identifier;
			qi::rule<Iterator, ast::identifier(), skipper<Iterator> > function_identifier;
			qi::rule<Iterator, ast::variable(), skipper<Iterator> > variable_definition;
			qi::rule<Iterator, std::list<ast::variable>(), skipper<Iterator> > argument_list;
			qi::rule<Iterator, std::list<ast::variable>(), skipper<Iterator> > return_list;
			qi::rule<Iterator, ast::function_declaration(), skipper<Iterator> > function_header;
			qi::rule<Iterator, ast::statement_list(), skipper<Iterator> > scoped_block;
			qi::rule<Iterator, ast::function(), skipper<Iterator> > function_definition;
		};
	}
}