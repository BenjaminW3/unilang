#pragma once

#include "purity_validator.hpp"

#include <boost/variant/apply_visitor.hpp>

namespace unilang 
{
	namespace semantic_validator
	{
		namespace purity_validator
		{
			bool isPure(long double const & )
			{
				return true;
			}
			bool isPure(unsigned int const & )
			{
				return true;
			}
			bool isPure(int const & )
			{
				return true;
			}
			bool isPure(bool const & )
			{
				return true;
			}
			bool isPure(ast::function_call const & x)
			{
				// FIXME: pure test for function itself

				// all parameters have to be pure
				for(ast::expression const & ex : x.arguments)
				{
					if(!isPure(ex)) {return false;}
				}

				return true;
			}
			bool isPure(ast::identifier const & )
			{
				return true;
			}
			bool isPure(ast::unaryOp const & x)
			{
				return isPure(x.operand_);
			}
			bool isPure(ast::variable_definition const & x)
			{
				// FIXME: constructor has to be pure!

				// all parameters have to be pure
				if(x.parameters.is_initialized())
				{
					for(ast::expression const & ex : x.parameters.get())
					{
						if(!isPure(ex)) {return false;}
					}
				}
				// it has to be non mutable
				return (!x.type.mutableQualifier);
			}
			bool isPure(ast::operation const & x)
			{
				return isPure(x.operand_);
			}
			bool isPure(ast::expression const & x)
			{
				if(!isPure(x.first)){return false;}
				// all rest have to be pure
				for(ast::operation const & op : x.rest)
				{
					if(!isPure(op)) {return false;}
				}
				return true;
			}
			bool isPure(ast::operand const & x)
			{
				switch(x.which())
				{
					case 0: return isPure(boost::get<long double>(x)); break;
					case 1: return isPure(boost::get<unsigned int>(x)); break;
					case 2: return isPure(boost::get<int>(x)); break;
					case 3: return isPure(boost::get<bool>(x)); break;
					case 4: return isPure(boost::get<ast::function_call>(x)); break;
					case 5: return isPure(boost::get<ast::identifier>(x)); break;
					case 6: return isPure(boost::get<ast::unaryOp>(x)); break;
					case 7: return isPure(boost::get<ast::variable_definition>(x)); break;
					case 8: return isPure(boost::get<ast::expression>(x)); break;
					default: throw std::runtime_error("undefined-expression"); break;
				}
			}
			bool isPure(ast::assignment const & )
			{
				return false;
			}
			bool isPure(ast::statement_list const & x)
			{
				for(ast::statement const & st : x)
				{
					if(!isPure(st)) {return false;}
				}
				return true;
			}
			bool isPure(ast::statement const & x)
			{
				switch(x.which())
				{
					case 0: return isPure(boost::get<ast::assignment>(x)); break;
					case 1: return isPure(boost::get<ast::expression>(x)); break;
					case 2: return isPure(boost::get<ast::if_statement>(x)); break;
					//case 3: return isPure(boost::get<ast::while_statement>(x)); break;
					//case 4: return isPure(boost::get<ast::return_statement>(x)); break;
					case 3: return isPure(boost::get<ast::statement_list>(x)); break;
					default: throw std::runtime_error("undefine-statement"); break;
				}
			}
			bool isPure(ast::if_statement const & x)
			{
				return isPure(x.condition) && isPure(x.then) && (!x.else_.is_initialized() || isPure(x.else_.get()));
			}
			//bool isPure(ast::while_statement const & x);
			//bool isPure(ast::return_statement const & x);
			bool isPure(ast::function_declaration const & x)
			{
				return false;
			}
			bool isPure(ast::function_definition const & x)
			{
				return false;
			}
		}
	}
}