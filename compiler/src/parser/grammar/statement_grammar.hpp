#pragma once
#include "identifier_grammar.hpp"
#include "expression_grammar.hpp"

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
		//  The statement grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct statement_grammar : qi::grammar<Iterator, ast::statement_list(), skipper<Iterator> >
		{
			statement_grammar(error_handler<Iterator>& error_handler, identifier_grammar<Iterator> const & identifierGrammar, expression_grammar<Iterator> const & expressionGrammar)
			  : statement_grammar::base_type(statement_list)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::_val_type _val;
				qi::lexeme_type lexeme;
				qi::alnum_type alnum;
				qi::lit_type lit;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				// a statement list consists of statements
				statement_list =
					*statement
					;
				statement_list.name("statement_list");

				statement =
						assignment
					|   variable_definition_statement
					|	compound_statement
					|   if_statement
					|   while_statement
				//	|   return_statement
					;
				statement.name("statement");

				variable_definition =
						identifierGrammar
					>	'='
					>	identifierGrammar
					>	'('
					>	-( expressionGrammar % ',')
					>	')'
					;
				variable_definition.name("variable_definition");

				variable_definition_statement =
						variable_definition
					>   ';'
					;
				variable_definition_statement.name("variable_definition_statement");

				assignment =
						identifierGrammar
					>   '='
					>   expressionGrammar
					>   ';'
					;
				assignment.name("assignment");

				if_statement =
						lit("if")
					>   '('
					>   expressionGrammar
					>   ')'
					>   statement
					>
					   -(
							lexeme["else" >> !(alnum | '_')] // make sure we have whole words
						>   statement
						)
					;
				if_statement.name("if_statement");

				while_statement =
						lit("while")
					>   '('
					>   expressionGrammar
					>   ')'
					>   statement
					;
				while_statement.name("while_statement");

				compound_statement =
						'{'
					>	-statement_list
					>	'}'
					;
				compound_statement.name("compound_statement");

				/*return_statement =
						lexeme["return" >> !(alnum | '_')] // make sure we have whole words
					>   ';'
					;
				return_statement.name("return_statement");*/

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(statement_list)
					(variable_definition)
					(variable_definition_statement)
					(assignment)
					(if_statement)
					(while_statement)
					(compound_statement)
				);

				// Error handling: on error in statement_list, call error_handler.
				on_error<fail>(statement_list,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in variable_declaration, assignment and return_statement, call annotation.
				/*on_success(variable_declaration,
					annotation_function(error_handler.iters)(_val, _1));
				on_success(assignment,
					annotation_function(error_handler.iters)(_val, _1));
				on_success(return_statement,
					annotation_function(error_handler.iters)(_val, _1));*/
			}

			qi::rule<Iterator, ast::statement_list(), skipper<Iterator> >
				statement_list, compound_statement;

			qi::rule<Iterator, ast::statement(), skipper<Iterator> > 
				statement;

			qi::rule<Iterator, ast::variable_definition(), skipper<Iterator> > 
				variable_definition, variable_definition_statement;

			qi::rule<Iterator, ast::assignment(), skipper<Iterator> > 
				assignment;

			qi::rule<Iterator, ast::if_statement(), skipper<Iterator> > 
				if_statement;

			qi::rule<Iterator, ast::while_statement(), skipper<Iterator> > 
				while_statement;

			/*qi::rule<Iterator, ast::return_statement(), skipper<Iterator> > 
				return_statement;*/
		};
	}
}