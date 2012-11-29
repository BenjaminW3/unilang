#include "exp_code_gen.hpp"

#include "../../ast/expression_ast.hpp"

#include "../../log/log.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/IRBuilder.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

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
				pRetVal = getBuilder()->CreateLoad(pDeclAlloca, "loadDefVal");
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
			if(!pDeclAlloca)
			{
				return ErrorValue("Unable to load variable definition value from invalid allocation.");
			}

			llvm::Value * pRetVal (getBuilder()->CreateLoad(pDeclAlloca, "loadDefVal"));
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