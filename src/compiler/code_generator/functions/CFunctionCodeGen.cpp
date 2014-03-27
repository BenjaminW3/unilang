#include <unilang/compiler/code_generator/functions/CFunctionCodeGen.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CFunctionCodeGen::CFunctionCodeGen(	
			CErrorCodeGen & codeGeneratorErrors,
			CModuleCodeGen & namespaceCodeGenerator,
			CLLVMCodeGen & llvmCodeGenerator,
			CSymbolCodeGen & symbolCodeGenerator,
			CAllocationCodeGen & allocationCodeGenerator,
			CExpressionCodeGen & expressionCodeGenerator,
			CStatementCodeGen & statementCodeGenerator) :
			m_codeGeneratorErrors		(codeGeneratorErrors),
			m_namespaceCodeGenerator	(namespaceCodeGenerator),
			m_llvmCodeGenerator			(llvmCodeGenerator),
			m_symbolCodeGenerator		(symbolCodeGenerator),
			m_allocationCodeGenerator	(allocationCodeGenerator),
			m_expressionCodeGenerator	(expressionCodeGenerator),
			m_statementCodeGenerator	(statementCodeGenerator)
		{
		}
	}
}