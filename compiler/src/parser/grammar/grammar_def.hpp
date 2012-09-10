#include "grammar.hpp"

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
		global_grammar<BaseIterator,Iterator>::global_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
																lexer::token_lexer<BaseIterator>& lexer)
			: global_grammar::base_type(module, "global_grammar"),
			identifierGrammar(error_handler, lexer),
			expressionGrammar(error_handler, identifierGrammar, lexer),
			statementGrammar(error_handler, /*identifierGrammar,*/ expressionGrammar, lexer),
			functionGrammar(error_handler, identifierGrammar, expressionGrammar, statementGrammar, lexer)
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

			metaEntityList	=	*( functionGrammar | functionGrammar.functionDeclaration );

			module			=	metaEntityList.alias();
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(metaEntityList)
				(module)
			);
#endif
			// Error handling: on error in start, call error_handler.
			on_error<fail>(metaEntityList,	error_handler_function(error_handler)( "Error! Expecting ", _4, _3));
			on_error<fail>(module,			error_handler_function(error_handler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			//on_success(metaEntityList,		annotation_function(error_handler.iters)(_val, _1));
			on_success(module,				annotation_function(error_handler.iters)(_val, _1));
		}
	}
}