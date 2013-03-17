#include "exp_code_gen.hpp"

#include "../types.hpp"
#include "../errors.hpp"
#include "../llvm/llvm_code_gen.hpp"

#include "../../ast/operators.hpp"
#include "../../ast/expression_ast.hpp"
#include "../../ast/operators_def.hpp"

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
		llvm::Value * expression_code_generator::operator()(ast::unary_expr const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value * const pVal (x._opOperand.apply_visitor(*this));
			if(!pVal)
			{
				std::stringstream sstr;
				sstr << x._opOperand;
				return m_codeGeneratorErrors.ErrorValue("Invalid value returned from '"+sstr.str()+"'");
			}

			switch (
#ifdef TOKEN_ID
				x._uiOperatorID
#else
				static_cast<operators::EOperators>(x._uiOperatorID)
#endif
				)
			{
			case operators::EOperators::minus:
				{
					return m_llvmCodeGenerator.getBuilder()->CreateNeg(pVal, "neg");
				}
			case operators::EOperators::not_:
				{
					return m_llvmCodeGenerator.getBuilder()->CreateNot(pVal, "not");
				}
			case operators::EOperators::compl:
				{
					llvm::Value * pValAllOnes = llvm::Constant::getAllOnesValue(pVal->getType());
					if(!pValAllOnes)
					{
						return m_codeGeneratorErrors.ErrorValue("Unable to generate value with all bits set inside of complement!", EErrorLevel::Internal);
					}
					return m_llvmCodeGenerator.getBuilder()->CreateXor(pVal, pValAllOnes, "compl.xor");
				}
			case operators::EOperators::plus_plus:
				{
					if(	x._opOperand.var.type()==typeid(ast::primary_expr) && 
						boost::get<ast::primary_expr>(x._opOperand.var).var.type()==typeid(ast::identifier))
					{
						ast::expression rightEx;
						if(pVal->getType()->isIntegerTy() || pVal->getType()->isFloatingPointTy())
						{
							// create the one as primary expression
							ast::expression ex;
							ex._firstOperand = ast::operand(ast::primary_expr(size_t(1)));
							// cast it to the type needed
							ast::variable_definition varDef;
							varDef._declaration._type._idfName = llvmTypeToUnilangTypeName(pVal->getType());
							varDef._declaration._type._bHasMutableQualifier = false;
							varDef._vParameterExpressions.push_back(ex);
							rightEx._firstOperand = ast::operand(varDef);
						}
						else
						{
							return m_codeGeneratorErrors.ErrorValue("Operator '++' is not available for value of type '"+getLLVMTypeName(pVal->getType())+"' !", EErrorLevel::Fatal);
						}

						auto const idf (boost::get<ast::identifier>(boost::get<ast::primary_expr>(x._opOperand.var).var));

						ast::assignment assign	(idf,				
#ifdef TOKEN_ID
												operators::EOperators::plus_assign,
#else
												static_cast<size_t>(operators::EOperators::plus_assign),
#endif
												rightEx);
						assign._id = x._id;
						assign._lhs._id = idf._id;
						return (*this)(assign);
					}
					else
					{
						return m_codeGeneratorErrors.ErrorValue("Operator '++' is only applicaple to identifiers.", EErrorLevel::Fatal);
					}
				}
			case operators::EOperators::minus_minus:
				{
					if(	x._opOperand.var.type()==typeid(ast::primary_expr) && 
						boost::get<ast::primary_expr>(x._opOperand.var).var.type()==typeid(ast::identifier))
					{
						ast::expression rightEx;
						if(pVal->getType()->isIntegerTy() || pVal->getType()->isFloatingPointTy())
						{
							// create the one as primary expression
							ast::expression ex;
							ex._firstOperand = ast::operand(ast::primary_expr(size_t(1)));
							// cast it to the type needed
							ast::variable_definition varDef;
							varDef._declaration._type._idfName = llvmTypeToUnilangTypeName(pVal->getType());
							varDef._declaration._type._bHasMutableQualifier = false;
							varDef._vParameterExpressions.push_back(ex);
							rightEx._firstOperand = ast::operand(varDef);
						}
						else
						{
							return m_codeGeneratorErrors.ErrorValue("Operator '--' is not available for value of type '"+getLLVMTypeName(pVal->getType())+ "' !", EErrorLevel::Fatal);
						}

						auto const idf (boost::get<ast::identifier>(boost::get<ast::primary_expr>(x._opOperand.var).var));

						ast::assignment assign	(idf,				
#ifdef TOKEN_ID
												operators::EOperators::plus_assign,
#else
												static_cast<size_t>(operators::EOperators::minus_assign),
#endif
												rightEx);
						assign._id = x._id;
						assign._lhs._id = idf._id;
						return (*this)(assign);
					}
					else
					{
						return m_codeGeneratorErrors.ErrorValue("Operator '--' is only applicaple to identifiers.", EErrorLevel::Fatal);
					}
				}
			case operators::EOperators::plus:
				{
					return pVal; // + does not change anything
				}
			case operators::EOperators::stringify:
				{
					//throw std::runtime_error("Not implemented!");

					// FIXME: char conversion only for last digit!
					if(pVal->getType()->isDoubleTy())
					{
						// convert to int
						llvm::Value * const pUi (m_llvmCodeGenerator.getBuilder()->CreateFPToUI(pVal, llvm::IntegerType::get(m_llvmCodeGenerator.getContext(), 8), "FpToUiTmp"));
						if(!pUi)
						{
							return m_codeGeneratorErrors.ErrorValue("CreateFPToUI returned invalid value!", EErrorLevel::Internal);
						}
						// get last digit through reminder
						llvm::Value * const pPosLastDigit (m_llvmCodeGenerator.getBuilder()->CreateURem(pUi, (*this)(uint64_t(10)), "URemTmp"));
						if(!pPosLastDigit)
						{
							return m_codeGeneratorErrors.ErrorValue("CreateURem returned invalid value!", EErrorLevel::Internal);
						}
						// ascii
						return m_llvmCodeGenerator.getBuilder()->CreateAdd(pPosLastDigit, (*this)(uint64_t(3*16)), "AddTmp");
					}
					else if(pVal->getType()->isIntegerTy())
					{
						// get last digit through reminder
						llvm::Value * const pPosLastDigit (m_llvmCodeGenerator.getBuilder()->CreateURem(pVal, (*this)(uint64_t(10)), "URemTmp"));
						if(!pPosLastDigit)
						{
							return m_codeGeneratorErrors.ErrorValue("CreateURem returned invalid value!", EErrorLevel::Internal);
						}
						// ascii
						return m_llvmCodeGenerator.getBuilder()->CreateAdd(pPosLastDigit, (*this)(uint64_t(3*16)), "AddTmp");
					}
					else
					{
						return m_codeGeneratorErrors.ErrorValue("String conversion for type '"+getLLVMTypeName(pVal->getType())+"' is not implemented!", EErrorLevel::Fatal);
					}
					/*return getBuilder()->CreateGlobalString(L, "stringifytmp");*/
				}
			default:
				{
					std::stringstream sstr;
					sstr << 
#ifdef TOKEN_ID
					x._uiOperatorID
#else
					static_cast<operators::EOperators>(x._uiOperatorID)
#endif
					;
					return m_codeGeneratorErrors.ErrorValue("Unknown operation '"+sstr.str()+"'!", EErrorLevel::Fatal);
				}
			}
		}
	}
}