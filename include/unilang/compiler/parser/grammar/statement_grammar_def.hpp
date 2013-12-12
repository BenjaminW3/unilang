#pragma once

#include <unilang/compiler/parser/grammar/statement_grammar.hpp>

#include <unilang/compiler/parser/grammar/expression_grammar.hpp>

#include <unilang/compiler/ast/statement_ast.hpp>
#include <unilang/compiler/ast/fusion_adapt/statement_ast.hpp>

#include <unilang/compiler/lexer/lexer.hpp>
#include <unilang/compiler/parser/error_handler.hpp>
#include <unilang/compiler/parser/annotation.hpp>

#include <unilang/compiler/parser/spirit.hpp>

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		
		//-----------------------------------------------------------------------------
		//! Constructor.
		//-----------------------------------------------------------------------------
		template <typename BaseIterator, typename LexerIterator>
		statement_grammar<BaseIterator,LexerIterator>::statement_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
																			//identifier_grammar<BaseIterator, LexerIterator> const & identifierGrammar, 
																			expression_grammar<BaseIterator, LexerIterator> const & expressionGrammar, 
																			lexer::token_lexer<BaseIterator> const & lexer)
			: statement_grammar::base_type(m_ruleStatementList, "statement_grammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, LexerIterator> > error_handler_function;
			typedef function<unilang::annotation<LexerIterator> > annotation_function;

			// expression as a statement is needed for calling functions without storing a return value
			m_ruleExpressionStatement =
					expressionGrammar
				>	lexer(";")
				;
			m_ruleExpressionStatement.name("expressionStatement");
			
			m_ruleAssignmentStatement =
					expressionGrammar.m_ruleAssignmentExpression
				>	lexer(";")
				;
			m_ruleAssignmentStatement.name("assignmentStatement");

			m_ruleIfStatement =
					lexer("if")
				>	lexer("\\(")
				>	expressionGrammar
				>	lexer("\\)")
				>	m_ruleCompoundStatement
				>	-(
						lexer("else")
					>	m_ruleCompoundStatement
					)
				;
			m_ruleIfStatement.name("ifStatement");

			/*m_ruleWhileStatement =
					lexer("while")
				>   lexer("\\(")
				>   expressionGrammar
				>   lexer("\\)")
				>	m_ruleCompoundStatement
				;
			m_ruleWhileStatement.name("whileStatement");*/


			/*m_ruleReturnStatement =
					lexer("return") 
				>	-expressionGrammar
				>   lexer(";")
				;
			m_ruleReturnStatement.name("returnStatement");*/

			m_ruleCompoundStatement =
					lexer("\\{")
				>>	m_ruleStatementList
				>	lexer("\\}")
				;
			m_ruleCompoundStatement.name("compoundStatement");

			m_ruleStatement =
					m_ruleIfStatement
			//	|   m_ruleWhileStatement
			//	|   m_ruleReturnStatement
				|	m_ruleAssignmentStatement
				|  	m_ruleExpressionStatement
				|	m_ruleCompoundStatement
				;
			m_ruleStatement.name("statement");

			// a statement list consists of statements
			m_ruleStatementList =
				*m_ruleStatement
				;
			m_ruleStatementList.name("statementList");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_ruleExpressionStatement)
				(m_ruleAssignmentStatement)
				(m_ruleIfStatement)
				//(m_ruleWhileStatement)
				//(m_ruleReturnStatement)
				(m_ruleCompoundStatement)
				(m_ruleStatement)
				(m_ruleStatementList)
			);
#endif
			// Error handling: on error in statement_vector, call error_handler.
			qi::on_error<qi::fail>(m_ruleExpressionStatement,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleAssignmentStatement,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleIfStatement,			error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			//qi::on_error<qi::fail>(m_ruleWhileStatement,		error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			//qi::on_error<qi::fail>(m_ruleReturnStatement,		error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleCompoundStatement,		error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleStatement,				error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleStatementList,			error_handler_function(error_handler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			qi::on_success(m_ruleExpressionStatement,		annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(m_ruleAssignmentStatement,		annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(m_ruleIfStatement,				annotation_function(error_handler.iters)(_val, _1));
			/*qi::on_success(m_ruleWhileStatement,			annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(m_ruleReturnStatement,			annotation_function(error_handler.iters)(_val, _1));*/
			qi::on_success(m_ruleCompoundStatement,			annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(m_ruleStatement,					annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(m_ruleStatementList,				annotation_function(error_handler.iters)(_val, _1));
		}
	}
}