#include "alloc_code_gen.hpp"

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

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

#if defined(_MSC_VER)
#pragma warning(push)
#endif

#include "../types.hpp"

#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"
//#include "../constants/constants_code_gen.hpp"
#include "../symbols/symbol_code_gen.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		allocation_code_generator::allocation_code_generator(	code_generator_errors & codeGeneratorErrors,
																llvm_code_generator & llvmCodeGenerator,
																symbol_code_generator & symbolCodeGenerator,
																expression_code_generator & expressionCodeGenerator )
			:m_codeGeneratorErrors		(codeGeneratorErrors),
			m_llvmCodeGenerator			(llvmCodeGenerator),
			m_symbolCodeGenerator		(symbolCodeGenerator),
			m_expressionCodeGenerator	(expressionCodeGenerator)
		{
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::AllocaInst * allocation_code_generator::createEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & sVarName)
		{
			llvm::IRBuilder<> TmpB(&TheFunction->getEntryBlock(), TheFunction->getEntryBlock().begin());
			std::string const sTempName = "EntryBlockAlloca_" + sVarName;
			return TmpB.CreateAlloca(pType, 0,  sTempName.c_str());
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value * allocation_code_generator::create_cast(llvm::Value * pVal, llvm::Type * const pDestinationType)
		{
			if(!pVal)
			{
				return m_codeGeneratorErrors.ErrorValue("Invalid pInitVal for cast.", EErrorLevel::Internal);
			}
			if(!pDestinationType)
			{
				return m_codeGeneratorErrors.ErrorValue("Invalid pDestinationType for cast.", EErrorLevel::Internal);
			}

			llvm::Type * const pInitType (pVal->getType());

			/*if(llvm::CastInst::isCastable(pVal->getType(), pDestinationType))
			{
				llvm::Instruction::CastOps castOp (llvm::CastInst::getCastOpcode(pVal, true, pDestinationType, true));

				llvm::CastInst * castInst (llvm::CastInst::Create(castOp, pVal, pDestinationType, "FirstClassTypeCast", getBuilder()->GetInsertPoint()));
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
							pVal = m_llvmCodeGenerator.getBuilder()->CreateFPToSI(pVal, pDestinationType, "FPtoSI");
							if(!pVal)
							{
								return m_codeGeneratorErrors.ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
							}
						}
						else
						{
							return m_codeGeneratorErrors.ErrorValue("Can not cast value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.");
						}
					}

					assert(pVal->getType()->isIntegerTy());

					// at this point both types are at least integers of unknown precision
					if(pVal->getType() != pDestinationType)
					{
						llvm::Type * const pInitIntegerType (pVal->getType());

						// could use getScalarSizeInBits() that returns bit size for scalars directly and element bit size for vector types
						if(pVal->getType()->getScalarSizeInBits()==1) // boolean is unsigned
						{
							// We cannot have a truncation here because we are casting from bool to int
							pVal = m_llvmCodeGenerator.getBuilder()->CreateZExt(pVal, (llvm::IntegerType*) pDestinationType, "cast.bool.int");
						}
						else
						{
							pVal = m_llvmCodeGenerator.getBuilder()->CreateSExtOrTrunc(pVal, (llvm::IntegerType*) pDestinationType, "cast.int.int");
						}

						if(!pVal)
						{
							std::string const strIntermediateCast ((pInitType != pInitIntegerType) ? (" during intermediate cast from '"+getLLVMTypeName(pInitIntegerType)+"' to type '"+getLLVMTypeName(pInitType))+"'" : "");
							return m_codeGeneratorErrors.ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'"+strIntermediateCast+".", EErrorLevel::Internal);
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
							if(pVal->getType()->getScalarSizeInBits()==1) // boolean is unsigned
							{
								pVal = m_llvmCodeGenerator.getBuilder()->CreateUIToFP(pVal, pDestinationType, "SItoFP");
							}
							else
							{
								pVal = m_llvmCodeGenerator.getBuilder()->CreateSIToFP(pVal, pDestinationType, "SItoFP");
							}

							if(!pVal)
							{
								return m_codeGeneratorErrors.ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.", EErrorLevel::Internal);
							}
						}
						else
						{
							return m_codeGeneratorErrors.ErrorValue("Can not cast value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.");
						}
					}

					assert(pVal->getType()->isFloatingPointTy());
					
					// at this point both types are at least integers of unknown precision
					if(pVal->getType() != pDestinationType)
					{
						llvm::Type * const pInitIntegerType (pVal->getType());
						// could use getScalarSizeInBits() that returns bit size for scalars directly and element bit size for vector types
						// we could use CreateSExtOrTrunc instead but we want to generate extra downcast warnings.
						pVal = m_llvmCodeGenerator.getBuilder()->CreateFPCast(pVal, pDestinationType, "FloatCast");
						if(!pVal)
						{
							std::string const strIntermediateCast ((pInitType != pInitIntegerType) ? (" during intermediate cast from '"+getLLVMTypeName(pInitIntegerType)+"' to type '"+getLLVMTypeName(pInitType))+"'" : "");
							return m_codeGeneratorErrors.ErrorValue("Failure during cast of value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'"+strIntermediateCast+".", EErrorLevel::Internal);
						}
					}
				}
				else
				{
					return m_codeGeneratorErrors.ErrorValue("Can not cast value of type '"+getLLVMTypeName(pInitType)+"' to type '"+ getLLVMTypeName(pDestinationType) +"'.");
				}
			}
			return pVal;
		}
	}
}