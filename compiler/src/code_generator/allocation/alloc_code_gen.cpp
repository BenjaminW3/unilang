#include "alloc_code_gen.hpp"

#include "../../log/log.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#endif

#include <llvm/Function.h>

#if defined(_MSC_VER)
#pragma warning(push)
#endif

#include "../types.hpp"

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst *allocation_code_generator::ErrorAllocaInst(std::string Str, EErrorLevel err )
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" ALLOCATION: "+Str);
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst * allocation_code_generator::createEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName)
		{
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(),
			TheFunction->getEntryBlock().begin());
			std::string const sTempName = "EntryBlockAlloca_" + VarName;
			return TmpB.CreateAlloca(pType, 0,  sTempName.c_str());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * allocation_code_generator::create_cast(llvm::Value * pVal, llvm::Type * const pDestinationType)
		{
			if(!pVal)
			{
				return ErrorValue("Invalid pInitVal for cast.", EErrorLevel::Internal);
			}
			if(!pDestinationType)
			{
				return ErrorValue("Invalid pDestinationType for cast.", EErrorLevel::Internal);
			}

			llvm::Type * const pInitType (pVal->getType());

			/*if(llvm::CastInst::isCastable(pVal->getType(), pDestinationType))
			{
				llvm::Instruction::CastOps castOp (llvm::CastInst::getCastOpcode(pVal, true, pDestinationType, true));

				llvm::CastInst * castInst (llvm::CastInst::Create(castOp, pVal, pDestinationType, "FirstClassTypeCast", builder.GetInsertPoint()));
				pVal = castInst;
				if(!pVal)
				{
					return ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
				}
			}*/

			if(pVal->getType() != pDestinationType)
			{
				if(pDestinationType->isIntegerTy())
				{
					if(!pVal->getType()->isIntegerTy())
					{
						// if it is a fp type first convert it to int
						if(pVal->getType()->isFloatingPointTy())
						{	
							pVal = builder.CreateFPToSI(pVal, pDestinationType, "FPtoSI");
							if(!pVal)
							{
								return ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
							}
						}
						else
						{
							return ErrorValue("Can not cast value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.");
						}
					}

					assert(pVal->getType()->isIntegerTy());

					// at this point both types are at least integers of unknown precision
					if(pVal->getType() != pDestinationType)
					{
						// could use getScalarSizeInBits() that returns bit size for scalars directly and element bit size for vector types
						// we could use CreateSExtOrTrunc instead but we want to generate extra downcast warnings.
						pVal = builder.CreateSExtOrTrunc(pVal, (llvm::IntegerType*) pDestinationType, "IntCast");
						if(!pVal)
						{
							// TODO: different error message if pVal->getType!=pInitType
							return ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
						}
					}
				}
				else if(pDestinationType->isFloatingPointTy())
				{
					if(!pVal->getType()->isFloatingPointTy())
					{
						// convert if it is int
						if(pVal->getType()->isIntegerTy())
						{	
							pVal = builder.CreateSIToFP(pVal, pDestinationType, "SItoFP");
							if(!pVal)
							{
								return ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
							}
						}
						else
						{
							return ErrorValue("Can not cast value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.");
						}
					}

					assert(pVal->getType()->isFloatingPointTy());
					
					// at this point both types are at least integers of unknown precision
					if(pVal->getType() != pDestinationType)
					{
						// could use getScalarSizeInBits() that returns bit size for scalars directly and element bit size for vector types
						// we could use CreateSExtOrTrunc instead but we want to generate extra downcast warnings.
						pVal = builder.CreateFPCast(pVal, pDestinationType, "FloatCast");
						if(!pVal)
						{
							// TODO: different error message if pVal->getType!=pInitType
							return ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
						}
					}
				}
				else
				{
					return ErrorValue("Can not cast value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.");
				}
			}
			return pVal;
		}
	}
}