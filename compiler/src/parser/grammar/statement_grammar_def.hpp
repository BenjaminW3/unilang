#include "statement_grammar.hpp"

#include "expression_grammar.hpp"

#include "../../ast/statement_ast.hpp"
#include "../../ast/fusion_adapt/statement_ast.hpp"

#include "../../lexer/lexer.hpp"
#include "../error_handler.hpp"
#include "../annotation.hpp"

namespace unilang 
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		
		//-----------------------------------------------------------------------------
		//! Constructor.
		//-----------------------------------------------------------------------------
		template <typename BaseIterator, typename Iterator>
		statement_grammar<BaseIterator,Iterator>::statement_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
																		//identifier_grammar<BaseIterator, Iterator> const & identifierGrammar, 
																		expression_grammar<BaseIterator, Iterator> const & expressionGrammar, 
																		lexer::token_lexer<BaseIterator>& lexer)
			: statement_grammar::base_type(statementList, "statement_grammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using qi::on_error;
			using qi::on_success;
			using qi::fail;
			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, Iterator> > error_handler_function;
			typedef function<unilang::annotation<Iterator> > annotation_function;

			expressionStatement =
					expressionGrammar
				>	lexer(";")
				;
			expressionStatement.name("expressionStatement");

			ifStatement =
					lexer("if")
				>	lexer("\\(")
				>	expressionGrammar
				>	lexer("\\)")
				>	compoundStatement
				>	-(
						lexer("else")
					>	compoundStatement
					)
				;
			ifStatement.name("ifStatement");

			/*whileStatement =
					lexer("while")
				>   lexer("\\(")
				>   expressionGrammar
				>   lexer("\\)")
				>	compoundStatement
				;
			whileStatement.name("whileStatement");*/


			/*returnStatement =
					lexer("return") 
				>	-expressionGrammar
				>   lexer(";")
				;
			returnStatement.name("returnStatement");*/

			compoundStatement =
					lexer("\\{")
				>>	statementList
				>	lexer("\\}")
				;
			compoundStatement.name("compoundStatement");

			statement =
					ifStatement
			//	|   whileStatement
			//	|   returnStatement
				|  	expressionStatement
				|	compoundStatement
				;
			statement.name("statement");

			// a statement list consists of statements
			statementList =
				*statement
				;
			statementList.name("statementList");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(expressionStatement)
				(ifStatement)
				//(whileStatement)
				//(returnStatement)
				(compoundStatement)
				(statement)
				(statementList)
			);
#endif
			// Error handling: on error in statement_list, call error_handler.
			on_error<fail>(expressionStatement,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			on_error<fail>(ifStatement,			error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			//on_error<fail>(whileStatement,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			//on_error<fail>(returnStatement,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			on_error<fail>(compoundStatement,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			on_error<fail>(statement,			error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			on_error<fail>(statementList,		error_handler_function(error_handler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			on_success(expressionStatement,		annotation_function(error_handler.iters)(_val, _1));
			on_success(ifStatement,				annotation_function(error_handler.iters)(_val, _1));
			/*on_success(whileStatement,		annotation_function(error_handler.iters)(_val, _1));
			on_success(returnStatement,			annotation_function(error_handler.iters)(_val, _1));*/
			on_success(compoundStatement,		annotation_function(error_handler.iters)(_val, _1));
			on_success(statement,				annotation_function(error_handler.iters)(_val, _1));
			on_success(statementList,			annotation_function(error_handler.iters)(_val, _1));
		}
	}
}