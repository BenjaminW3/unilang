#pragma once

#include <unilang/compiler/parser/grammar/StatementGrammarFwd.hpp>

#include <unilang/compiler/parser/grammar/ExpressionGrammarFwd.hpp>

#include <unilang/compiler/ast/Statement.hpp>
#include <unilang/compiler/ast/fusion_adapt/Statement.hpp>

#include <unilang/compiler/lexer/LexerFwd.hpp>
#include <unilang/compiler/lexer/TokenIDsDef.hpp>
#include <unilang/compiler/parser/SourceErrorHandler.hpp>
#include <unilang/compiler/parser/AnnotationHandler.hpp>

#include <unilang/compiler/parser/Spirit.hpp>

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		
		//-------------------------------------------------------------------------
		//! Constructor.
		//-------------------------------------------------------------------------
		template <typename TSourceCodeIterator, typename TLexerTokenIterator>
		SStatementGrammar<TSourceCodeIterator,TLexerTokenIterator>::SStatementGrammar(
			SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator>& sourceErrorHandler, 
			//SIdentifierGrammar<TSourceCodeIterator, TLexerTokenIterator> const & identifierGrammar, 
			SExpressionGrammar<TSourceCodeIterator, TLexerTokenIterator> const & expressionGrammar, 
			lexer::CTokenLexer<TSourceCodeIterator> const & lexer) :
			SStatementGrammar::base_type(m_ruleStatementList, "SStatementGrammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using boost::phoenix::function;

			typedef function<unilang::SSourceErrorHandler<TSourceCodeIterator, TLexerTokenIterator> > TErrorHandlerFunction;
			typedef function<unilang::SAnnotationHandler<TLexerTokenIterator> > TAnnotationFunction;

			// expression as a statement is needed for calling functions without storing a return value
			m_ruleExpressionStatement =
					expressionGrammar
				>	lexer(lexer::tokens::ETokenIDs::semicolon)
				;
			m_ruleExpressionStatement.name("expressionStatement");
			
			m_ruleAssignmentStatement =
					expressionGrammar.m_ruleAssignmentExpression
				>	lexer(lexer::tokens::ETokenIDs::semicolon)
				;
			m_ruleAssignmentStatement.name("assignmentStatement");

			m_ruleIfStatement =
					lexer(lexer::tokens::ETokenIDs::if_)
				>	lexer(lexer::tokens::ETokenIDs::opening_parenthesis)
				>	expressionGrammar
				>	lexer(lexer::tokens::ETokenIDs::closing_parenthesis)
				>	m_ruleCompoundStatement
				>	-(
						lexer(lexer::tokens::ETokenIDs::else_)
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
					lexer(lexer::tokens::ETokenIDs::return_) 
				>	-expressionGrammar
				>   lexer(lexer::tokens::ETokenIDs::semicolon)
				;
			m_ruleReturnStatement.name("returnStatement");*/

			m_ruleCompoundStatement =
					lexer(lexer::tokens::ETokenIDs::opening_brace)
				>>	m_ruleStatementList
				>	lexer(lexer::tokens::ETokenIDs::closing_brace)
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
			// Error handling: on error in SStatementVector, call the error handler.
			qi::on_error<qi::fail>(m_ruleExpressionStatement,	TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleAssignmentStatement,	TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleIfStatement,			TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			//qi::on_error<qi::fail>(m_ruleWhileStatement,		TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			//qi::on_error<qi::fail>(m_ruleReturnStatement,		TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleCompoundStatement,		TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleStatement,				TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleStatementList,			TErrorHandlerFunction(sourceErrorHandler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotaion.
			qi::on_success(m_ruleExpressionStatement,		TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(m_ruleAssignmentStatement,		TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(m_ruleIfStatement,				TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			/*qi::on_success(m_ruleWhileStatement,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(m_ruleReturnStatement,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));*/
			qi::on_success(m_ruleCompoundStatement,			TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(m_ruleStatement,					TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
			qi::on_success(m_ruleStatementList,				TAnnotationFunction(sourceErrorHandler.iters)(_val, _1));
		}
	}
}