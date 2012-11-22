#include "../code_generator.hpp"

#include "../../ast/operators_def.hpp"

#include "../../log/log.hpp"

#include "llvm/Support/raw_ostream.h"

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
			VarData const * const V = getVarFromName(x.lhs.name);
			if(!V)
			{
				return ErrorValue("Undefined variable name '"+x.lhs.name+"' !");
			}
			else
			{
				if(!V->isMutable())
				{
					return ErrorValue("Assignment to const (non-mutable) variable '"+x.lhs.name+"' is impossible!");
				}
				else
				{
					llvm::AllocaInst *lhs = V->getAllocaInst();
					if(!lhs)
					{
						return ErrorValue("Variable '"+x.lhs.name+"' is not allocated!", EErrorLevel::Internal);
					}
					else
					{
						// Codegen the RHS.
						llvm::Value *rhs = (*this)(x.rhs);
						if(!rhs)
						{
							return ErrorValue("Invalid right hand side of an assignment!");
						}
						if(rhs->getType()!=lhs->getType()->getElementType())
						{
							std::string type_str;
							llvm::raw_string_ostream rso(type_str);
							rso << "Trying to assign a value of type '";
							rhs->getType()->print(rso);
							rso << "' to a value of type '";
							lhs->getType()->print(rso);
							rso << "'.";
							return ErrorValue("Assignment type mismatch! "+rso.str());
						}
						else
						{
							if(
#ifdef TOKEN_ID
				x.operator_
#else
				static_cast<operators::EOperators>(x.operator_)
#endif
								== operators::EOperators::assign)
							{
								/*return */builder.CreateStore(rhs, lhs);
								return rhs;
							}
							else // more then just an assignment
							{
								// remove all possible flags to get the pure operation
								size_t const opRemoveFlags(~(static_cast<size_t>(operators::EOperatorTypes::assignmentOperation) & static_cast<size_t>(operators::EOperatorTypes::binaryOperation) & static_cast<size_t>(operators::EOperatorTypes::unaryOperation)));
								operators::EOperators const opType = static_cast<operators::EOperators>(static_cast<size_t>(x.operator_) & opRemoveFlags);
								llvm::Value *CalcVal = CreateBinaryOperation(lhs, rhs, opType);
								if(!CalcVal)
								{
									return ErrorValue("Unable to compute result of operation prior to assignment!");
								}
								/*return */builder.CreateStore(CalcVal, lhs);
								return CalcVal;
							}
						}
					}
				}
			}
		}
	}
}