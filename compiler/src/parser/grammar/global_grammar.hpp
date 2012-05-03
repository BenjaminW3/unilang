#pragma once
#include "identifier_grammar.hpp"
#include "function_grammar.hpp"
#include "expression_grammar.hpp"
#include "statement_grammar.hpp"

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
		//  The function grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct global_grammar : qi::grammar<Iterator, ast::module(), skipper<Iterator> >
		{
			global_grammar(error_handler<Iterator>& error_handler)
			 : global_grammar::base_type(module),
			 identifierGrammar(error_handler),
			 expressionGrammar(error_handler, identifierGrammar),
			 statementGrammar(error_handler, identifierGrammar, expressionGrammar),
			 functionGrammar(error_handler, identifierGrammar, statementGrammar)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				metaEntityList	=	*( functionGrammar | functionGrammar.function_header | statementGrammar.variable_definition );

				module			=	metaEntityList;

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(module)
					(metaEntityList)
				);

				// Error handling: on error in start, call error_handler.
				on_error<fail>(module,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in start, call annotation.
				/*on_success(identifier,
					annotation_function(error_handler.iters)(_val, _1));*/
			}

			identifier_grammar<Iterator> identifierGrammar;
			function_grammar<Iterator> functionGrammar;
			expression_grammar<Iterator> expressionGrammar;
			statement_grammar<Iterator> statementGrammar;
			qi::rule<Iterator, std::list<ast::meta_entity>(), skipper<Iterator> > metaEntityList;
			qi::rule<Iterator, ast::module(), skipper<Iterator> > module;
		};
	}
}