#include "statement_code_gen.hpp"

#include "../errors.hpp"
#include "../expressions/exp_code_gen.hpp"

#include "../../ast/statement_ast.hpp"

#include "../../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		statement_code_generator::statement_code_generator(	code_generator_errors & codeGeneratorErrors,
															llvm_code_generator & llvmCodeGenerator,
															expression_code_generator & expressionCodeGenerator )
			:m_codeGeneratorErrors		(codeGeneratorErrors),
			m_llvmCodeGenerator			(llvmCodeGenerator),
			m_expressionCodeGenerator	(expressionCodeGenerator)
		{
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::expression const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return m_expressionCodeGenerator(x) != nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::assignment const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return m_expressionCodeGenerator(x) != nullptr;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::statement const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);
			return x.apply_visitor(*this);
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool statement_code_generator::operator()(ast::statement_vector const& x)
		{
			LOG_SCOPE_DEBUG;
			LOG(x);

			for(ast::statement const& s : x)
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