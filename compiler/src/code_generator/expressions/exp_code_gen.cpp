#include "exp_code_gen.hpp"

#include "../../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::primary_expr const& x)
		{
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		/*llvm::Value * expression_code_generator::operator()(ast::operand const & x)
		{
			LOG(x);
			// get the value
			llvm::Value * pRetVal = x.apply_visitor(*this);
			
			// if it was a definition we have to load the value from memory
			if(x.var.type()==typeid(ast::variable_definition))
			{
				llvm::AllocaInst * pDeclAlloca = (llvm::AllocaInst *) pRetVal;
				pRetVal = builder.CreateLoad(pDeclAlloca, "loadDefVal");
			}

			return pRetVal;
		}*/
		
		//-----------------------------------------------------------------------------
		// if it was a definition we have to load the value from memory
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(ast::variable_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			llvm::AllocaInst * pDeclAlloca ((*dynamic_cast<allocation_code_generator*>(this))(x));

			llvm::Value * pRetVal (builder.CreateLoad(pDeclAlloca, "loadDefVal"));
			if(!pRetVal)
			{
				return ErrorValue("Unable to load variable definition value.");
			}

			return pRetVal;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(long double const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(double const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(float const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(uint64_t const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(int64_t const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(uint32_t const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(int32_t const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * expression_code_generator::operator()(bool const & x)
		{
			return (*dynamic_cast<constants_code_generator*>(this))(x);
		}
	}
}