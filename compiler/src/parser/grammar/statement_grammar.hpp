#pragma once
#include "identifier_grammar.hpp"
#include "expression_grammar.hpp"

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
		//  The statement grammar
		///////////////////////////////////////////////////////////////////////////////
		template <typename Iterator>
		struct statement_grammar : qi::grammar<Iterator, ast::statement_list(), skipper<Iterator> >
		{
			statement_grammar(error_handler<Iterator>& error_handler, identifier_grammar<Iterator> const & identifierGrammar, expression_grammar<Iterator> const & expressionGrammar)
			  : statement_grammar::base_type(statementList)
			{
				qi::_1_type _1;
				qi::_2_type _2;
				qi::_3_type _3;
				qi::_4_type _4;

				qi::_val_type _val;
				qi::lexeme_type lexeme;
				qi::alnum_type alnum;
				qi::lit_type lit;
				qi::char_type char_;
				qi::omit_type omit;
				qi::string_type string;
				qi::raw_type raw;
				qi::eps_type eps;
				qi::bool_type bool_;

				using qi::on_error;
				using qi::on_success;
				using qi::fail;
				using boost::phoenix::function;
				using boost::phoenix::val;

				typedef function<unilang::error_handler<Iterator> > error_handler_function;
				//typedef function<unilang::annotation<Iterator> > annotation_function;

				constQualifier = 
						-bool_ //(char_)[_val = bool(val<std::string>("~") == val<std::string>(_1))]
					;
				constQualifier.name("constQualifier");

				variableDefinitionIdentifier = 
					-(
						':'
						>>	raw[identifierGrammar]
					)
					;
				variableDefinitionIdentifier.name("variableDefinitionIdentifier");
				
				parameterList = 
					-(
						'('
						>>	-( expressionGrammar % ',')
						>>	')'
					)
					;
				parameterList.name("parameterList");

				//∈↦
				// A Variable Definition is of the form <type-qualifier><type><identifier><initialisation-list>
				// <type-qualifier> can be '~'
				// <type> one of the predefined or user define types
				// <identifier> can be: -empty
				//						-a user defined identifier name surrounded by '[',']'
				// <initialisation-list> can be:	-empty, 
				//									-'()' 
				//									-a list of expressions seperated by ',' surrounded by '(',')'
				variableDefinition =
						constQualifier
					>>	identifierGrammar
					>>	variableDefinitionIdentifier
					>>	parameterList
					;
				variableDefinition.name("variableDefinition");

				assignment =
						identifierGrammar
					>>   '='
					>>	expressionGrammar
					>>   ';'
					;
				assignment.name("assignment");

				if_statement =
						lit("if")
					>>   '('
					>>   expressionGrammar
					>>   ')'
					>>   statement
					>>
					   -(
							lexeme["else" >> !(alnum | '_')] // make sure we have whole words
						>>   statement
						)
					;
				if_statement.name("if_statement");

				while_statement =
						lit("while")
					>>   '('
					>>   expressionGrammar
					>>   ')'
					>>   statement
					;
				while_statement.name("while_statement");

				compound_statement =
						'{'
					>>	-statementList
					>>	'}'
					;
				compound_statement.name("compound_statement");

				/*return_statement =
						lexeme["return" >> !(alnum | '_')] // make sure we have whole words
					>>   ';'
					;
				return_statement.name("return_statement");*/

				statement =
						assignment
					|   variableDefinition
					|	compound_statement
					|   if_statement
					|   while_statement
				//	|   return_statement
					;
				statement.name("statement");

				// a statement list consists of statements
				statementList =
					*statement
					;
				statementList.name("statementList");

				// Debugging and error handling and reporting support.
				BOOST_SPIRIT_DEBUG_NODES(
					(constQualifier)
					(variableDefinitionIdentifier)
					(parameterList)
					(variableDefinition)
					(assignment)
					(if_statement)
					(while_statement)
					(compound_statement)
					(statement)
					(statementList)
				);

				// Error handling: on error in statement_list, call error_handler.
				on_error<fail>(statementList,
					error_handler_function(error_handler)(
						"Error! Expecting ", _4, _3));

				// Annotation: on success in variable_declaration, assignment and return_statement, call annotation.
				/*on_success(variable_declaration,
					annotation_function(error_handler.iters)(_val, _1));
				on_success(assignment,
					annotation_function(error_handler.iters)(_val, _1));
				on_success(return_statement,
					annotation_function(error_handler.iters)(_val, _1));*/
			}
			
			qi::rule<Iterator, boost::optional<bool>(), skipper<Iterator> > constQualifier;
			qi::rule<Iterator, boost::optional<ast::identifier>(), skipper<Iterator> > variableDefinitionIdentifier;
			qi::rule<Iterator, boost::optional<std::list<ast::expression>>(), skipper<Iterator> > parameterList;
			qi::rule<Iterator, ast::variable_definition(), skipper<Iterator> > variableDefinition;

			qi::rule<Iterator, ast::statement_list(), skipper<Iterator> > 
				statementList, compound_statement;

			qi::rule<Iterator, ast::statement(), skipper<Iterator> > statement;

			qi::rule<Iterator, ast::assignment(), skipper<Iterator> > assignment;

			qi::rule<Iterator, ast::if_statement(), skipper<Iterator> > 
				if_statement;

			qi::rule<Iterator, ast::while_statement(), skipper<Iterator> > 
				while_statement;

			/*qi::rule<Iterator, ast::return_statement(), skipper<Iterator> > 
				return_statement;*/
		};
	}
}