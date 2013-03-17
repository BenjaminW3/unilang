#include "identifier_grammar.hpp"

#include "../../ast/identifier_ast.hpp"
#include "../../ast/fusion_adapt/identifier_ast.hpp"

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
		template <typename BaseIterator, typename LexerIterator>
		identifier_grammar<BaseIterator,LexerIterator>::identifier_grammar(	error_handler<BaseIterator, LexerIterator>& error_handler, 
																		lexer::token_lexer<BaseIterator> const & lexer)
			: identifier_grammar::base_type(m_ruleIdentifier, "identifier_grammar")
		{
			qi::_1_type _1;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;
			qi::matches_type matches;
			boost::spirit::eps_type eps;

			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, LexerIterator> > error_handler_function;
			typedef function<unilang::annotation<LexerIterator> > annotation_function;

			m_ruleIdentifier = lexer.m_tokIdentifier >> eps; // struct with one element needs eps http://boost.2283326.n4.nabble.com/BOOST-FUSION-ADAPT-STRUCT-with-single-entry-td2675332.html#a2675332
			m_ruleIdentifier.name("identifier");

			m_ruleNamespacedIdentifier = 
					matches[lexer("::")]
				>>	(lexer.m_tokIdentifier % lexer("::"));
			m_ruleNamespacedIdentifier.name("m_ruleNamespacedIdentifier");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(m_ruleIdentifier)
				(m_ruleNamespacedIdentifier)
			);
#endif
			// Error handling: on error in start, call error_handler.
			qi::on_error<qi::fail>(m_ruleIdentifier,			error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			qi::on_error<qi::fail>(m_ruleNamespacedIdentifier,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			qi::on_success(m_ruleIdentifier,			annotation_function(error_handler.iters)( _val, _1));
			qi::on_success(m_ruleNamespacedIdentifier,	annotation_function(error_handler.iters)( _val, _1));
		}
	}
}