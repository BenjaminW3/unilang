#include "function_grammar.hpp"

#include "../../lexer/lexer.hpp"
#include "../error_handler.hpp"
#include "../annotation.hpp"

#include "../spirit.hpp"

namespace unilang
{ 
	namespace parser
	{
		namespace qi = boost::spirit::qi;

		//-----------------------------------------------------------------------------
		//! Constructor
		//-----------------------------------------------------------------------------
		template <typename BaseIterator, typename Iterator>
		function_grammar<BaseIterator,Iterator>::function_grammar(	error_handler<BaseIterator, Iterator>& error_handler, 
																	identifier_grammar<BaseIterator, Iterator> const & identifierGrammar, 
																	expression_grammar<BaseIterator, Iterator> const & expressionGrammar, 
																	statement_grammar<BaseIterator, Iterator> const & statementGrammar, 
																	lexer::token_lexer<BaseIterator>& lexer)
			: function_grammar::base_type(functionDefinition, "function_grammar")
		{
			qi::_1_type _1;
			//qi::_2_type _2;
			qi::_3_type _3;
			qi::_4_type _4;

			//qi::_val_type _val;
			qi::string_type string;
			qi::matches_type matches;

			using qi::on_error;
			using qi::on_success;
			using qi::fail;
			using boost::phoenix::function;

			typedef function<unilang::error_handler<BaseIterator, Iterator> > error_handler_function;
			//typedef function<unilang::annotation<Iterator> > annotation_function;

			parameterDeclarationList =
					'('
				>>	-( expressionGrammar.typeDeclaration % ',')
				>>	')'
				;
			parameterDeclarationList.name("parameterDeclarationList");

			returnDeclarationList =
					'('
				>>	-( expressionGrammar.typeDeclaration % ',')
				>>	')'
				;
			returnDeclarationList.name("returnDeclarationList");

			functionDeclaration =
					'?'
				>>	matches['~']
				>>	parameterDeclarationList
				>>	lexer("->")
				>>  returnDeclarationList
				>>	':'
				>>	identifierGrammar
				;
			functionDeclaration.name("functionDeclaration");

			parameterDefinitionList =
					'('
				>>	-( expressionGrammar.variableDeclaration % ',')
				>>	')'
				;
			parameterDefinitionList.name("parameterDefinitionList");

			returnDefinitionList =
					'('
				>>	-( expressionGrammar.variableDefinition % ',')
				>>	')'
				;
			returnDefinitionList.name("returnDefinitionList");

			functionDefinition =
					matches['~']
				>>	parameterDefinitionList
				>>	lexer("->")
				>>  returnDefinitionList
				>>	':'
				>>	identifierGrammar
				>>	-statementGrammar.compoundStatement
				;
			functionDefinition.name("functionDefinition");
#ifdef _DEBUG
			// Debugging and error handling and reporting support.
			BOOST_SPIRIT_DEBUG_NODES(
				(parameterDeclarationList)
				(returnDeclarationList)
				(functionDeclaration)
				(parameterDefinitionList)
				(returnDefinitionList)
				(functionDefinition)
			);
#endif
			// Error handling: on error in start, call error_handler.
			on_error<fail>(functionDefinition, error_handler_function(error_handler)("Error! Expecting ", _4, _3));

			// Annotation: on success, call annotation.
			/*on_success(identifier, annotation_function(error_handler.iters)(_val, _1));*/
		}
	}
}