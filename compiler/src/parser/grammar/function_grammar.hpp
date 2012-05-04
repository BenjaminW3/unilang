#pragma once
#include "identifier_grammar.hpp"
#include "statement_grammar.hpp"

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
		struct function_grammar : qi::grammar<Iterator, ast::function(), skipper<Iterator> >
		{
			function_grammar(error_handler<Iterator>& error_handler, identifier_grammar<Iterator> const & identifierGrammar, statement_grammar<Iterator> const & statementGrammar)
			 : function_grammar::base_type(function_definition)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::_val_type _val;
				qi::string_type string;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				argument_list =
						'('
					>	-( statementGrammar.variable_definition % ',')
					>	')'
					;
				argument_list.name("argument_list");

				return_list =
						'('
					>	-( statementGrammar.variable_definition % ',')
					>	')'
					;
				return_list.name("return_list");

				function_header =
						identifierGrammar
					>	argument_list
					>	':'
					>   return_list
					;
				function_header.name("function_header");

				scoped_block =
						'{'
					>	statementGrammar
					>	'}'
					;
				scoped_block.name("scoped_block");

				function_definition =
						identifierGrammar
					>	argument_list
					>	':'
					>   return_list
					>   scoped_block
					;
				function_definition.name("function_definition");

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(argument_list)
					(return_list)
					(function_header)
					(scoped_block)
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
			
			qi::rule<Iterator, std::list<ast::variable_definition>(), skipper<Iterator> > argument_list;
			qi::rule<Iterator, std::list<ast::variable_definition>(), skipper<Iterator> > return_list;
			qi::rule<Iterator, ast::function_declaration(), skipper<Iterator> > function_header;
			qi::rule<Iterator, ast::statement_list(), skipper<Iterator> > scoped_block;
			qi::rule<Iterator, ast::function(), skipper<Iterator> > function_definition;
		};
	}
}