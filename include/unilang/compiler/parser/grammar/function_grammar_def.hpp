#pragma once

#include <unilang/compiler/parser/grammar/function_grammar.hpp>

#include <unilang/compiler/parser/grammar/identifier_grammar.hpp>
#include <unilang/compiler/parser/grammar/expression_grammar.hpp>
#include <unilang/compiler/parser/grammar/statement_grammar.hpp>

#include <unilang/compiler/ast/function_ast.hpp>
#include <unilang/compiler/ast/fusion_adapt/function_ast.hpp>

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
		function_grammar<BaseIterator,LexerIterator>::function_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
																	identifier_grammar<BaseIterator, LexerIterator> const & identifierGrammar, 
																	expression_grammar<BaseIterator, LexerIterator> const & expressionGrammar, 
																	statement_grammar<BaseIterator, LexerIterator> const & statementGrammar, 
																	lexer::token_lexer<BaseIterator> const & lexer) :
			function_grammar::base_type(m_ruleFunctionDefinition, "function_grammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, LexerIterator> > error_handler_function;
			typedef function<unilang::annotation<LexerIterator> > annotation_function;

			m_ruleParameterDeclarationList =
					lexer("\\(")
				>	-( expressionGrammar.m_ruleTypeDeclaration % lexer(","))
				>	lexer("\\)")
				;
			m_ruleParameterDeclarationList.name("parameterDeclarationList");

			m_ruleReturnDeclarationList =
					lexer("\\(")
				>	-( expressionGrammar.m_ruleTypeDeclaration % lexer(","))
				>	lexer("\\)")
				;
			m_ruleReturnDeclarationList.name("returnDeclarationList");

			m_ruleFunctionDeclaration =
					lexer("\\?")
				>	identifierGrammar
				>	lexer(":")
				>	expressionGrammar.m_ruleMutableQualifier
				>	m_ruleParameterDeclarationList
				>	lexer("->")
				>	m_ruleReturnDeclarationList
				;
			m_ruleFunctionDeclaration.name("functionDeclaration");

			m_ruleParameterDefinitionList =
					lexer("\\(")
				>	-( expressionGrammar.m_ruleVariableDeclaration % lexer(","))
				>	lexer("\\)")
				;
			m_ruleParameterDefinitionList.name("parameterDefinitionList");

			m_ruleReturnDefinitionList =
					lexer("\\(")
				>	-( expressionGrammar.m_ruleVariableDefinition % lexer(","))
				>	lexer("\\)")
				;
			m_ruleReturnDefinitionList.name("returnDefinitionList");

			m_ruleFunctionDefinition =
					identifierGrammar
				>	lexer(":")
				>	expressionGrammar.m_ruleMutableQualifier
				>	m_ruleParameterDefinitionList
				>	lexer("->")
				>	m_ruleReturnDefinitionList
				>	-statementGrammar.m_ruleCompoundStatement
				;
			m_ruleFunctionDefinition.name("functionDefinition");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_ruleParameterDeclarationList)
				(m_ruleReturnDeclarationList)
				(m_ruleFunctionDeclaration)
				(m_ruleParameterDefinitionList)
				(m_ruleReturnDefinitionList)
				(m_ruleFunctionDefinition)
			);
#endif
			// Error handling: on error in start, call error_handler.
			qi::on_error<qi::fail>(m_ruleParameterDeclarationList,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleReturnDeclarationList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleFunctionDeclaration,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleParameterDefinitionList,	error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleReturnDefinitionList,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleFunctionDefinition,		error_handler_function(error_handler)("Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			//qi::on_success(m_ruleParameterDeclarationList,	annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			//qi::on_success(m_ruleReturnDeclarationList,		annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(m_ruleFunctionDeclaration,			annotation_function(error_handler.iters)(_val, _1));
			//qi::on_success(m_ruleParameterDefinitionList,		annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			//qi::on_success(m_ruleReturnDefinitionList,		annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(m_ruleFunctionDefinition,			annotation_function(error_handler.iters)(_val, _1));
		}
	}
}