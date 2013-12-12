#pragma once

#include <unilang/compiler/parser/grammar/grammar.hpp>

#include <unilang/compiler/ast/ast.hpp>
#include <unilang/compiler/ast/fusion_adapt/ast.hpp>

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
		global_grammar<BaseIterator,LexerIterator>::global_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
																	lexer::token_lexer<BaseIterator> const & lexer)
			: global_grammar::base_type(m_ruleModule, "global_grammar"),
			identifierGrammar(error_handler, lexer),
			expressionGrammar(error_handler, identifierGrammar, lexer),
			statementGrammar(error_handler, /*identifierGrammar,*/ expressionGrammar, lexer),
			functionGrammar(error_handler, identifierGrammar, expressionGrammar, statementGrammar, lexer)
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;

			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, LexerIterator> > error_handler_function;
			typedef function<unilang::annotation<LexerIterator> > annotation_function;

			m_rulevMetaEntities	=	*( functionGrammar | functionGrammar.m_ruleFunctionDeclaration | m_ruleNamespaceDeclaration );
			m_rulevMetaEntities.name("metaEntities");

			m_ruleModule		=	m_rulevMetaEntities.alias();
			m_ruleModule.name("module");

			m_ruleNamespaceDeclaration = 
					lexer("namespace") 
				>	lexer(":")
				>	identifierGrammar
				>	lexer("\\{")
				>	m_rulevMetaEntities
				>	lexer("\\}")
				;
			m_ruleNamespaceDeclaration.name("namespaceDeclaration");

#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_rulevMetaEntities)
				(m_ruleModule)
				(m_ruleNamespaceDeclaration)
			);
#endif
			// Error handling: on error in start, call error_handler.
			qi::on_error<qi::fail>(m_rulevMetaEntities,				error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleModule,					error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleNamespaceDeclaration,		error_handler_function(error_handler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			//qi::on_success(m_rulevMetaEntities,				annotation_function(error_handler.iters)(_val, _1));	// no success, this is no ast element
			qi::on_success(m_ruleModule,						annotation_function(error_handler.iters)(_val, _1));
			qi::on_success(m_ruleNamespaceDeclaration,			annotation_function(error_handler.iters)(_val, _1));
		}
	}
}