#include <unilang/compiler/code_generator/code_generator.hpp>

#include <unilang/compiler/ast/ast.hpp>

#include <unilang/compiler/log/log.hpp>

#include <stdio.h>

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		void code_generator::addStringConverters()
		{
			/*std::vector<llvm::Type*> Doubles(	x._vArgumentExpressions.size(),	llvm::Type::getDoubleTy(getContext()));
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::get(getContext()),	Doubles, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x._idfName._name, module.get());

			llvm::Constant *double_to_string_func = module->getOrInsertFunction("double_to_string",
												llvm::Type::getVoidTy(),
												llvm::PointerType::getUnqual(llvm::Type::Int8Ty),
												llvm::Type::Int32Ty,
												llvm::Type::Int32Ty,
												nullptr);

			llvm::Value* v = llvm::ConstantArray::get(llvmContext, myString.c_str());*/

			//llvm::ConstantFP::getInfinity();

			//getModule()->;
		}

		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator::code_generator(ast::module const & AST) :
			m_codeGeneratorErrors		(),
			m_namespaceCodeGenerator	(m_codeGeneratorErrors, m_functionCodeGenerator),
			m_llvmCodeGenerator			(m_codeGeneratorErrors, m_namespaceCodeGenerator),
			m_constantsCodeGenerator	(m_llvmCodeGenerator),
			m_symbolCodeGenerator		(m_codeGeneratorErrors, m_llvmCodeGenerator),
			m_allocationCodeGenerator	(m_codeGeneratorErrors, m_llvmCodeGenerator, m_symbolCodeGenerator, m_expressionCodeGenerator),
			m_expressionCodeGenerator	(m_codeGeneratorErrors, m_namespaceCodeGenerator, m_llvmCodeGenerator, m_constantsCodeGenerator, m_symbolCodeGenerator, m_allocationCodeGenerator),
			m_statementCodeGenerator	(m_codeGeneratorErrors, m_llvmCodeGenerator, m_expressionCodeGenerator),
			m_functionCodeGenerator		(m_codeGeneratorErrors, m_namespaceCodeGenerator, m_llvmCodeGenerator, m_symbolCodeGenerator, m_allocationCodeGenerator, m_statementCodeGenerator)
		{
			LOG_SCOPE_DEBUG;
			
			std::cout << std::endl << "##########CodeGen###########" << std::endl;
			
			// generate code
			bool const bSuccess (m_namespaceCodeGenerator(AST));

			// test success
			if(!bSuccess || m_codeGeneratorErrors.getErrorOccured())
			{
				throw std::runtime_error("Error occured during compilation of module!");
			}
			std::cout << "############################" << std::endl << std::endl;
			
			// throws excpetion if there are errors
			m_llvmCodeGenerator.verifyModule();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm_code_generator const & code_generator::getllvmCodeGenerator() const
		{
			return m_llvmCodeGenerator;
		}
	}
}