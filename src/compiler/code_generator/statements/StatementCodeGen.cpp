#include <unilang/compiler/code_generator/statements/StatementCodeGen.hpp>

#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>
#include <unilang/compiler/code_generator/expressions/ExpressionCodeGen.hpp>

#include <unilang/compiler/ast/Statement.hpp>

#include <unilang/compiler/log/log.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CStatementCodeGen::CStatementCodeGen(
			CErrorCodeGen & codeGeneratorErrors,
			CLLVMCodeGen & llvmCodeGenerator,
			CExpressionCodeGen & expressionCodeGenerator ) :
			m_codeGeneratorErrors		(codeGeneratorErrors),
			m_llvmCodeGenerator			(llvmCodeGenerator),
			m_expressionCodeGenerator	(expressionCodeGenerator)
		{
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CStatementCodeGen::operator()(ast::SExpression const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return m_expressionCodeGenerator(x) != nullptr;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CStatementCodeGen::operator()(ast::SAssignment const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return m_expressionCodeGenerator(x) != nullptr;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CStatementCodeGen::operator()(ast::SStatement const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CStatementCodeGen::operator()(ast::SStatementVector const & x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			for(ast::SStatement const & s : x)
			{
				if (!s.apply_visitor(*this))
				{
					return m_codeGeneratorErrors.ErrorBool("Invalid statement!");
				}
			}
			return true;
		}
	}
}