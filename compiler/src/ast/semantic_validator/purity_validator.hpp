#pragma once

#include "../ast.hpp"

namespace unilang 
{
	namespace semantic_validator
	{
		namespace purity_validator
		{
			bool isPure(long double const & x);
			bool isPure(unsigned int const & x);
			bool isPure(int const & x);
			bool isPure(bool const & x);
			bool isPure(ast::function_call const & x);
			bool isPure(ast::identifier const & x);
			bool isPure(ast::unaryOp const & x);
			bool isPure(ast::type_declaration const & x);
			bool isPure(ast::variable_declaration const & x);
			bool isPure(ast::variable_definition const & x);
			bool isPure(ast::operation const & x);
			bool isPure(ast::expression const & x);
			bool isPure(ast::operand const & x);
			bool isPure(ast::assignment const & x);
			bool isPure(ast::statement_list const & x);
			bool isPure(ast::statement const & x);
			bool isPure(ast::if_statement const & x);
			//bool isPure(ast::while_statement const & x);
			//bool isPure(ast::return_statement const & x);
			bool isPure(ast::function_declaration const & x);
			bool isPure(ast::function_definition const & x);
		}
	}
}