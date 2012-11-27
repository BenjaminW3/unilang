#include "exp_code_gen.hpp"

#include "../../ast/operators_def.hpp"

#include "../../log/log.hpp"

#include "../types.hpp"

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

#include <llvm/Type.h>

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
		llvm::Value * expression_code_generator::operator()(ast::assignment const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			// Look up the name.
			VarData const * const V = getVarFromName(x._lhs._name);
			if(!V)
			{
				return ErrorValue("Undefined variable name '"+x._lhs._name+"' !");
			}
			else
			{
				if(!V->isMutable())
				{
					return ErrorValue("Assignment to const (non-mutable) variable '"+x._lhs._name+"' is impossible!");
				}
				else
				{
					llvm::AllocaInst *lhsAlloca = V->getAllocaInst();
					if(!lhsAlloca)
					{
						return ErrorValue("Variable '"+x._lhs._name+"' is not allocated!", EErrorLevel::Internal);
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *rhsVal = (*this)(x._rhs);
						if(!rhsVal)
						{
							return ErrorValue("Invalid right hand side of an assignment!");
						}
						if(rhsVal->getType()!=lhsAlloca->getType()->getElementType())
						{
							return ErrorValue("Assignment type mismatch! Trying to assign a value of type '" +getLLVMTypeName(rhsVal->getType())+ 
												"' to a value of type '"+getLLVMTypeName(lhsAlloca->getType())+"'.");
						}
						else
						{
							if(
#ifdef TOKEN_ID
				x._operator
#else
				static_cast<operators::EOperators>(x._operator)
#endif
								== operators::EOperators::assign)
							{
								/*return */getBuilder()->CreateStore(rhsVal, lhsAlloca);
								return rhsVal;
							}
							else // more _then just an assignment
							{
								llvm::Value * pVal (getBuilder()->CreateLoad(lhsAlloca, "loadAssignLHSVal"));
								if(!pVal)
								{
									return ErrorValue("Unable to load LHS variable value in combined assignment.");
								}

								// remove all possible flags to get the pure operation
								size_t const opRemoveFlags(~static_cast<size_t>(operators::EOperatorTypes::assignmentOperation));
								operators::EOperators const opType = static_cast<operators::EOperators>(static_cast<size_t>(x._operator) & opRemoveFlags);
								llvm::Value *CalcVal = CreateBinaryOperation(pVal, rhsVal, opType);
								if(!CalcVal)
								{
									return ErrorValue("Unable to compute result of operation prior to assignment!");
								}
								/*return */getBuilder()->CreateStore(CalcVal, lhsAlloca);
								return CalcVal;
							}
						}
					}
				}
			}
		}
	}
}