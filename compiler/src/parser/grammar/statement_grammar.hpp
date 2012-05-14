#pragma once
#include "identifier_grammar.hpp"
#include "expression_grammar.hpp"

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
		//  The statement grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct statement_grammar : qi::grammar<Iterator, ast::statement_list(), skipper<Iterator> >
		{
			statement_grammar(error_handler<Iterator>& error_handler, identifier_grammar<Iterator> const & identifierGrammar, expression_grammar<Iterator> const & expressionGrammar)
			  : statement_grammar::base_type(statementList, "statement_grammar")
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::lexeme_type lexeme;
				qi::alnum_type alnum;
				qi::lit_type lit;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;
				using boost::phoenix::val;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				assignmentStatement =
						identifierGrammar
					>>   '='
					>>	expressionGrammar
					>>   ';'
					;
				assignmentStatement.name("assignmentStatement");
				
				expressionStatement =
						expressionGrammar
					>>	';'
					;
				expressionStatement.name("expressionStatement");

				ifStatement =
						lit("if")
					>>	'('
					>>	expressionGrammar
					>>	')'
					>>	'{'
					>>   statementList
					>>	'}'
					>>
					   -(
							lexeme["else" >> !(alnum | '_')] // make sure we have whole words
						>>	'{'
						>>   statementList
						>>	'}'
						)
					;
				ifStatement.name("ifStatement");

				/*whileStatement =
						lit("while")
					>>   '('
					>>   expressionGrammar
					>>   ')'
					>>   statementlist
					;
				whileStatement.name("whileStatement");*/


				/*returnStatement =
						lexeme["return" >> !(alnum | '_')] // make sure we have whole words
					>>   ';'
					;
				returnStatement.name("returnStatement");*/

				compoundStatement =
						'{'
					>>	-statementList
					>>	'}'
					;
				compoundStatement.name("compoundStatement");

				statement =
						assignmentStatement
					|	expressionStatement
					|   ifStatement
				//	|   whileStatement
				//	|   returnStatement
					|	compoundStatement
					;
				statement.name("statement");

				// a statement list consists of statements
				statementList =
					*statement
					;
				statementList.name("statementList");

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(assignmentStatement)
					(expressionStatement)
					(ifStatement)
					//(whileStatement)
					//(returnStatement)
					(compoundStatement)
					(statement)
					(statementList)
				);

				// Error handling: on error in statement_list, call error_handler.
				on_error<fail>(statementList,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in variable_declaration, assignment and returnStatement, call annotation.
				/*on_success(variable_declaration,
					annotation_function(error_handler.iters)(_val, _1));
				on_success(assignment,
					annotation_function(error_handler.iters)(_val, _1));
				on_success(returnStatement,
					annotation_function(error_handler.iters)(_val, _1));*/
			}

			qi::rule<Iterator, ast::assignment(), skipper<Iterator> > assignmentStatement;
			qi::rule<Iterator, ast::expression(), skipper<Iterator> > expressionStatement;
			qi::rule<Iterator, ast::if_statement(), skipper<Iterator> > ifStatement;
			/*qi::rule<Iterator, ast::while_statement(), skipper<Iterator> > whileStatement;
			qi::rule<Iterator, ast::return_statement(), skipper<Iterator> > returnStatement;*/
			qi::rule<Iterator, ast::statement(), skipper<Iterator> > statement;

			qi::rule<Iterator, ast::statement_list(), skipper<Iterator> > statementList, compoundStatement;

		};
	}
}