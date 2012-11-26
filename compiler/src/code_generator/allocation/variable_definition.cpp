#include "alloc_code_gen.hpp"

#include "../expressions/exp_code_gen.hpp"

#include "../../log/log.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst * allocation_code_generator::operator()(ast::variable_definition const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			
			bool const bHasName		(x._declaration._optionalIdentifier.is_initialized());

			// Emit the initializer before adding the variable to scope, this prevents the initializer from referencing the variable itself
			llvm::Value * pInitVal	(nullptr);
			
			// TODO: hard coded types
			std::string const sTypeName		(x._declaration._type._identifier._name);
			llvm::Type * const pDeclType	(getTypeByName(sTypeName));

			if(pDeclType->isIntegerTy())
			{
				if(x._lParameterExpressions.size()==0)
				{
					if(sTypeName == "i64")
					{
						pInitVal = (*dynamic_cast<constants_code_generator*>(this))(int64_t(0));
					}
					else if(sTypeName == "i32")
					{
						pInitVal = (*dynamic_cast<constants_code_generator*>(this))(int32_t(0));
					}
					if (!pInitVal)
					{
						std::stringstream sstr;
						sstr << *x._lParameterExpressions.begin();
						auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
						return ErrorAllocaInst("InitVal for variable '"+sVarName+"' of type '"+sTypeName+"' could not be generated.");
					}
				}
				else if(x._lParameterExpressions.size()==1)
				{
					pInitVal = _exp_code_gen(*x._lParameterExpressions.begin());
					if (!pInitVal)
					{
						std::stringstream sstr;
						sstr << *x._lParameterExpressions.begin();
						auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
						// TODO: output real parameter number
						return ErrorAllocaInst("Invalid 1. initialisation parameter '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+sTypeName+"'");
					}
					else if(pInitVal->getType() != pDeclType)
					{
						pInitVal = create_cast(pInitVal, pDeclType);
						if(!pInitVal)
						{
							std::stringstream sstr;
							sstr << *x._lParameterExpressions.begin();
							auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
							return ErrorAllocaInst("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+sTypeName+"' can not be cast.");
						}
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x._lParameterExpressions.size();
					return ErrorAllocaInst("Variable of type '"+sTypeName+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}
			else if(pDeclType->isFloatingPointTy())
			{
				if(x._lParameterExpressions.size()==0)
				{
					// If not specified, use 0.0.
					if(sTypeName == "f64")
					{
						pInitVal = (*dynamic_cast<constants_code_generator*>(this))(double(0.0));
					}
					else if(sTypeName == "f32")
					{
						pInitVal = (*dynamic_cast<constants_code_generator*>(this))(float(0.0));
					}
					if (!pInitVal)
					{
						std::stringstream sstr;
						sstr << *x._lParameterExpressions.begin();
						auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
						return ErrorAllocaInst("InitVal for variable '"+sVarName+"' of type '"+sTypeName+"' could not be generated.");
					}
				}
				else if(x._lParameterExpressions.size()==1)
				{
					pInitVal = _exp_code_gen(*x._lParameterExpressions.begin());
					if (!pInitVal)
					{
						std::stringstream sstr;
						sstr << *x._lParameterExpressions.begin();
						auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
						return ErrorAllocaInst("Invalid 1. initialisation parameter'"+sstr.str()+"' for variable '"+sVarName+"' of type '"+sTypeName+"'");
					}
					else if(pInitVal->getType() != pDeclType)
					{
						pInitVal = create_cast(pInitVal, pDeclType);
						if(!pInitVal)
						{
							std::stringstream sstr;
							sstr << *x._lParameterExpressions.begin();
							auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
							return ErrorAllocaInst("Expression '"+sstr.str()+"' for variable '"+sVarName+"' of type '"+sTypeName+"' can not be cast.");
						}
					}
				}
				else
				{
					std::stringstream sstr;
					sstr << x._lParameterExpressions.size();
					return ErrorAllocaInst("Variable of type '"+sTypeName+"' can not be initialized with more then 1 parameters ("+sstr.str()+" given).");
				}
			}
			else
			{
				return ErrorAllocaInst("Unknown type '"+sTypeName+"' during variable definition.");
			}

			// build declaration (allocation, etc.)
			llvm::AllocaInst * pDeclAlloc ((*this)(x._declaration));
			if(!pDeclAlloc)
			{
				auto const sVarName (bHasName ? x._declaration._optionalIdentifier.get()._name : "");
				return ErrorAllocaInst("Unable to declare variable '"+sVarName+"' of type '"+sTypeName+"'.");
			}

			builder.CreateStore(pInitVal, pDeclAlloc);

			return pDeclAlloc;
		}
	}
}