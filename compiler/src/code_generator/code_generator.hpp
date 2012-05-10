#pragma once

#include "../ast/ast.hpp"

#include <boost/variant/apply_visitor.hpp>

#include <vector>
#include <map>
#include <memory>

#pragma warning(disable: 4996)		// unsafe usage of std::copy

#pragma warning(push)
#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4355)		// "this" in initializer list
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// 'return' : conversion from 'int' to 'unsigned int', signed/unsigned mismatch
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// 'unsigned int' : forcing value to bool 'true' or 'false' (performance warning)

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/DerivedTypes.h"

namespace unilang 
{ 
	namespace code_generator
	{
		///////////////////////////////////////////////////////////////////////////
		//  The Compiler
		///////////////////////////////////////////////////////////////////////////
		class code_generator : public boost::static_visitor<llvm::Value *>
		{
		public:
			code_generator(ast::module const & AST);
			
		private:
			//-----------------------------------------------------------------------------
			//! Create an alloca instruction in the entry block of the function.  
			//! This is used for mutable variables etc.
			//-----------------------------------------------------------------------------
			static llvm::AllocaInst * CreateEntryBlockAlloca(llvm::Function *TheFunction, const std::string &VarName);

		public:
			//llvm::Value * operator()(ast::null const &);
			llvm::Value * operator()(unsigned int const & x);
			llvm::Value * operator()(int const & x);
			llvm::Value * operator()(bool const & x);
			llvm::Value * operator()(ast::identifier const & x);
			llvm::Value * operator()(ast::unaryOp const & x);
			llvm::Value * operator()(ast::binaryOp const & x);
			llvm::Value * operator()(ast::function_call const & x);
			llvm::Value * operator()(ast::expression const & x);
			llvm::Value * operator()(ast::assignment const & x);
			llvm::Value * operator()(ast::variable_definition const & x);
			llvm::Value * operator()(ast::statement_list const & x);
			llvm::Value * operator()(ast::statement const & x);
			llvm::Value * operator()(ast::if_statement const & x);
			//llvm::Value * operator()(ast::while_statement const & x);
			//llvm::Value * operator()(ast::return_statement const & x);
			llvm::Function * operator()(ast::function_declaration const & x);
			llvm::Function * operator()(ast::function const & x);
			//llvm::Value * operator()(ast::module const & x);

			void print_assembler() const;

			//std::shared_ptr<function> find_function(std::string const& name) const;

		private:
			llvm::LLVMContext & context;
			llvm::IRBuilder<> builder;

			std::unique_ptr<llvm::Module> module;
			
			class VarData
			{
			public:
				VarData(std::string identifier, llvm::AllocaInst* pMem, bool bConst)
					: m_identifier(identifier),
					m_pMem(pMem),
					m_bConst(bConst)
				{
					if(m_identifier.empty())
					{
						throw std::invalid_argument("Empty identifier is invalid for a variable!");
					}
				}
				std::string const & getIdentifier() const {return m_identifier;}
				llvm::AllocaInst* getAllocaInst() const {return m_pMem;}
				bool isConst() const {return m_bConst;}

			private:
				std::string m_identifier;
				llvm::AllocaInst* m_pMem;
				bool m_bConst;
			};

			std::vector<llvm::Value*> retValues;
			std::vector<VarData> symbolTable;

			VarData * getVarFromName( std::string const & name );
		};
	}
}
#pragma warning(pop)