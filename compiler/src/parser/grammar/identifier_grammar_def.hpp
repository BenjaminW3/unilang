#include "identifier_grammar.hpp"

#include "../../lexer/lexer.hpp"
#include "../error_handler.hpp"
#include "../annotation.hpp"

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;
		
		//-----------------------------------------------------------------------------
		//! Constructor
		//-----------------------------------------------------------------------------
		template <typename BaseIterator, typename Iterator>
		identifier_grammar<BaseIterator,Iterator>::identifier_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
																		lexer::token_lexer<BaseIterator>& lexer)
			: identifier_grammar::base_type(identifier, "identifier_grammar")
		{
			qi::_1_type _1;
			qi::_2_type _2;
			qi::_3_type _3;
			qi::_4_type _4;
			qi::_val_type _val;
			boost::spirit::eps_type eps;

			using qi::on_error;
			using qi::on_success;
			using qi::fail;
			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, Iterator> > error_handler_function;
			typedef function<unilang::annotation<Iterator> > annotation_function;

			identifier = lexer.tok_identifier >> eps; // struct with one element needs eps http://boost.2283326.n4.nabble.com/BOOST-FUSION-ADAPT-STRUCT-with-single-entry-td2675332.html#a2675332
			identifier.name("identifier");

#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(identifier)
			);
#endif
			// Error handling: on error in start, call error_handler.
			on_error<fail>(identifier, error_handler_function(error_handler)( "Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			//on_success(identifier, annotation_function(error_handler.iters)( _val, _1));
		}
	}
}