#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>

#include <unilang/compiler/code_generator/Types.hpp>
#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/llvm/LLVMCodeGen.hpp>

#include <unilang/compiler/ast/OperatorsFwd.hpp>
#include <unilang/compiler/ast/Expression.hpp>
#include <unilang/compiler/ast/OperatorsDef.hpp>

#include <unilang/compiler/log/log.hpp>

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

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value * CExpressionCodeGen::operator()(ast::SUnaryExpr const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			llvm::Value * const pVal (x.m_opOperand.apply_visitor(*this));
			if(!pVal)
			{
				std::stringstream sstr;
				sstr << x.m_opOperand;
				return m_codeGeneratorErrors.getErrorValue("Invalid value returned from '"+sstr.str()+"'");
			}

			switch (
#ifdef TOKEN_ID
				x.m_uiOperatorID
#else
				static_cast<operators::EOperators>(x.m_uiOperatorID)
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
						return m_codeGeneratorErrors.getErrorValue("Unable to generate value with all bits set inside of complement!", EErrorLevel::Internal);
					}
					return m_llvmCodeGenerator.getBuilder()->CreateXor(pVal, pValAllOnes, "compl.xor");
				}
			case operators::EOperators::plus_plus:
				{
					if(	x.m_opOperand.var.type()==typeid(ast::SPrimaryExpr) && 
						boost::get<ast::SPrimaryExpr>(x.m_opOperand.var).var.type()==typeid(ast::SIdentifier))
					{
						ast::SExpression rightEx;
						if(pVal->getType()->isIntegerTy() || pVal->getType()->isFloatingPointTy())
						{
							// create the one as primary expression
							ast::SExpression ex;
							ex.m_firstOperand = ast::SOperand(ast::SPrimaryExpr(size_t(1)));
							// cast it to the type needed
							ast::SVariableDefinition varDef;
							varDef.m_declaration.m_type.m_idfName = llvmTypeToUnilangTypeName(pVal->getType());
							varDef.m_declaration.m_type.m_bHasMutableQualifier = false;
							varDef.m_vParameterExpressions.push_back(ex);
							rightEx.m_firstOperand = ast::SOperand(varDef);
						}
						else
						{
							return m_codeGeneratorErrors.getErrorValue("Operator '++' is not available for value of type '"+getLLVMTypeName(pVal->getType())+"' !", EErrorLevel::Fatal);
						}

						auto const idf(boost::get<ast::SIdentifier>(boost::get<ast::SPrimaryExpr>(x.m_opOperand.var).var));

						ast::SAssignment assign	(idf,				
#ifdef TOKEN_ID
												operators::EOperators::plus_assign,
#else
												static_cast<size_t>(operators::EOperators::plus_assign),
#endif
												rightEx);
						assign.m_uiID = x.m_uiID;
						assign.m_lhs.m_uiID = idf.m_uiID;
						return (*this)(assign);
					}
					else
					{
						return m_codeGeneratorErrors.getErrorValue("Operator '++' is only applicaple to identifiers.", EErrorLevel::Fatal);
					}
				}
			case operators::EOperators::minus_minus:
				{
					if(	x.m_opOperand.var.type()==typeid(ast::SPrimaryExpr) && 
						boost::get<ast::SPrimaryExpr>(x.m_opOperand.var).var.type()==typeid(ast::SIdentifier))
					{
						ast::SExpression rightEx;
						if(pVal->getType()->isIntegerTy() || pVal->getType()->isFloatingPointTy())
						{
							// create the one as primary expression
							ast::SExpression ex;
							ex.m_firstOperand = ast::SOperand(ast::SPrimaryExpr(size_t(1)));
							// cast it to the type needed
							ast::SVariableDefinition varDef;
							varDef.m_declaration.m_type.m_idfName = llvmTypeToUnilangTypeName(pVal->getType());
							varDef.m_declaration.m_type.m_bHasMutableQualifier = false;
							varDef.m_vParameterExpressions.push_back(ex);
							rightEx.m_firstOperand = ast::SOperand(varDef);
						}
						else
						{
							return m_codeGeneratorErrors.getErrorValue("Operator '--' is not available for value of type '"+getLLVMTypeName(pVal->getType())+ "' !", EErrorLevel::Fatal);
						}

						auto const idf(boost::get<ast::SIdentifier>(boost::get<ast::SPrimaryExpr>(x.m_opOperand.var).var));

						ast::SAssignment assign(idf,
#ifdef TOKEN_ID
												operators::EOperators::plus_assign,
#else
												static_cast<size_t>(operators::EOperators::minus_assign),
#endif
												rightEx);
						assign.m_uiID = x.m_uiID;
						assign.m_lhs.m_uiID = idf.m_uiID;
						return (*this)(assign);
					}
					else
					{
						return m_codeGeneratorErrors.getErrorValue("Operator '--' is only applicaple to identifiers.", EErrorLevel::Fatal);
					}
				}
			case operators::EOperators::plus:
				{
					return pVal; // + does not change anything
				}
			case operators::EOperators::stringify:
				{
					//throw std::runtime_error("Not implemented!");

					// FIXME: Implement stringify!
					if(pVal->getType()->isDoubleTy())
					{
						return m_codeGeneratorErrors.getErrorValue("String conversion for type '"+getLLVMTypeName(pVal->getType())+"' is not implemented!", EErrorLevel::Fatal);
					}
					else if(pVal->getType()->isIntegerTy())
					{
						return m_codeGeneratorErrors.getErrorValue("String conversion for type '"+getLLVMTypeName(pVal->getType())+"' is not implemented!", EErrorLevel::Fatal);
					}
					else
					{
						return m_codeGeneratorErrors.getErrorValue("String conversion for type '"+getLLVMTypeName(pVal->getType())+"' is not implemented!", EErrorLevel::Fatal);
					}
				}
			default:
				{
					std::stringstream sstr;
					sstr << 
#ifdef TOKEN_ID
					x.m_uiOperatorID
#else
					static_cast<operators::EOperators>(x.m_uiOperatorID)
#endif
					;
					return m_codeGeneratorErrors.getErrorValue("Unknown operation '"+sstr.str()+"'!", EErrorLevel::Fatal);
				}
			}
		}
	}
}