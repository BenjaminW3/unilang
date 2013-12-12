#include <unilang/compiler/code_generator/functions/function_code_gen.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		function_code_generator::function_code_generator(	code_generator_errors & codeGeneratorErrors,
															namespace_code_generator & namespaceCodeGenerator,
															llvm_code_generator & llvmCodeGenerator,
															symbol_code_generator & symbolCodeGenerator,
															allocation_code_generator & allocationCodeGenerator,
															statement_code_generator & statementCodeGenerator)
			:m_codeGeneratorErrors		(codeGeneratorErrors),
			m_namespaceCodeGenerator	(namespaceCodeGenerator),
			m_llvmCodeGenerator			(llvmCodeGenerator),
			m_symbolCodeGenerator		(symbolCodeGenerator),
			m_allocationCodeGenerator	(allocationCodeGenerator),
			m_statementCodeGenerator	(statementCodeGenerator)
		{
		}
	}
}