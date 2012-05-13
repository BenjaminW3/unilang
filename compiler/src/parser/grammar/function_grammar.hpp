#pragma once
#include "identifier_grammar.hpp"
#include "statement_grammar.hpp"

#include "../../ast/ast.hpp"

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
			function_grammar(error_handler<Iterator>& error_handler, identifier_grammar<Iterator> const & identifierGrammar, expression_grammar<Iterator> const & expressionGrammar, statement_grammar<Iterator> const & statementGrammar)
			 : function_grammar::base_type(functionDefinition)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::_val_type _val;
				qi::string_type string;
				qi::omit_type omit;
				qi::matches_type matches;
				qi::lit_type lit;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				parameterDeclarationList =
						'('
					>>	-( expressionGrammar.variableDefinition % ',')
					>>	')'
					;
				parameterDeclarationList.name("parameterDeclarationList");

				returnList =
						'('
					>>	-( expressionGrammar.variableDefinition % ',')
					>>	')'
					;
				returnList.name("returnList");

				functionHeader =
						'<'
					>>	parameterDeclarationList
					>>	omit[string("->")]
					>>   returnList
					>>	'>'
					>>	identifierGrammar
					>>	matches['=']
					;
				functionHeader.name("functionHeader");

				functionDeclaration = 
						'?'
					>>	functionHeader
					;
				functionDeclaration.name("functionDeclaration");

				functionDefinition =
						functionHeader
					>>   -statementGrammar.compoundStatement
					;
				functionDefinition.name("functionDefinition");

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(parameterDeclarationList)
					(returnList)
					(functionHeader)
					(functionDeclaration)
					(functionDefinition)
				);

				// Error handling: on error in start, call error_handler.
				on_error<fail>(functionDefinition,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in start, call annotation.
				/*on_success(identifier,
					annotation_function(error_handler.iters)(_val, _1));*/
			}
			
			qi::rule<Iterator, std::list<ast::variable_definition>(), skipper<Iterator> > parameterDeclarationList;
			qi::rule<Iterator, std::list<ast::variable_definition>(), skipper<Iterator> > returnList;
			qi::rule<Iterator, ast::function_declaration(), skipper<Iterator> > functionHeader;
			qi::rule<Iterator, ast::function_declaration(), skipper<Iterator> > functionDeclaration;
			qi::rule<Iterator, ast::function(), skipper<Iterator> > functionDefinition;
		};
	}
}