#include <unilang/compiler/code_generator/CodeGenerator.hpp>

#include <unilang/compiler/ast/AST.hpp>

#include <unilang/compiler/log/log.hpp>

#include <iostream>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*void CCodeGenerator::addStringConverters()
		{
			std::vector<llvm::Type*> Doubles(	x.m_vArgumentExpressions.size(),	llvm::Type::getDoubleTy(getContext()));
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::get(getContext()),	Doubles, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x.m_idfName.m_sName, module.get());

			llvm::Constant *double_to_string_func = module->getOrInsertFunction("double_to_string",
												llvm::Type::getVoidTy(),
												llvm::PointerType::getUnqual(llvm::Type::Int8Ty),
												llvm::Type::Int32Ty,
												llvm::Type::Int32Ty,
												nullptr);

			llvm::Value* v = llvm::ConstantArray::get(llvmContext, myString.c_str());

			//llvm::ConstantFP::getInfinity();

			//getModule()->;
		}*/

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CCodeGenerator::CCodeGenerator(ast::SModule const & AST) :
			m_codeGeneratorErrors		(),
			m_namespaceCodeGenerator	(m_codeGeneratorErrors, m_functionCodeGenerator),
			m_llvmCodeGenerator			(m_codeGeneratorErrors, m_namespaceCodeGenerator),
			m_constantsCodeGenerator	(m_llvmCodeGenerator),
			m_symbolCodeGenerator		(m_codeGeneratorErrors, m_llvmCodeGenerator),
			m_allocationCodeGenerator	(m_codeGeneratorErrors, m_llvmCodeGenerator, m_symbolCodeGenerator, m_expressionCodeGenerator),
			m_expressionCodeGenerator	(m_codeGeneratorErrors, m_namespaceCodeGenerator, m_llvmCodeGenerator, m_constantsCodeGenerator, m_symbolCodeGenerator, m_allocationCodeGenerator),
			m_statementCodeGenerator	(m_codeGeneratorErrors, m_llvmCodeGenerator, m_expressionCodeGenerator),
			m_functionCodeGenerator		(m_codeGeneratorErrors, m_namespaceCodeGenerator, m_llvmCodeGenerator, m_symbolCodeGenerator, m_allocationCodeGenerator, m_expressionCodeGenerator, m_statementCodeGenerator)
		{
			LOG_SCOPE_DEBUG;
			
			std::cout << std::endl << "##########CodeGen###########" << std::endl;
			
			// Generate code.
			bool const bSuccess (m_namespaceCodeGenerator(AST));

			// Test if it succeeded.
			if(!bSuccess || m_codeGeneratorErrors.getErrorOccured())
			{
				throw std::runtime_error("Error occured during compilation of module!");
			}
			std::cout << "############################" << std::endl << std::endl;
			
			// Throw excpetion if there was an error.
			m_llvmCodeGenerator.verifyModule();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CLLVMCodeGen const & CCodeGenerator::getllvmCodeGenerator() const
		{
			return m_llvmCodeGenerator;
		}
	}
}