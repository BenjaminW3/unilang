#pragma once

#include "../ast/ast.hpp"

#include <boost/variant/apply_visitor.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
//#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
//#pragma warning(disable: 4355)		// "this" in initializer list
#pragma warning(disable: 4244)		// 'argument' : conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// 'return' : conversion from 'int' to 'unsigned int', signed/unsigned mismatch
#pragma warning(disable: 4512)		// 'llvm::IRBuilderBase' : assignment operator could not be generated
#pragma warning(disable: 4800)		// 'unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#endif

#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/IRBuilder.h"
#include "llvm/DerivedTypes.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <vector>
#include <map>
#include <memory>

namespace unilang 
{ 
	namespace code_generator
	{
		//#########################################################################
		//! Generates the code from an AST.
		//#########################################################################
		class code_generator : public boost::static_visitor<llvm::Value *>
		{
		public:
			code_generator(ast::module const & AST);
			
		private:
			void addStringConverters();

			//-----------------------------------------------------------------------------
			//! Create an alloca instruction in the entry block of the function.  
			//! This is used for mutable variables etc.
			//-----------------------------------------------------------------------------
			static llvm::AllocaInst * CreateEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName);

			bool m_bErrorOccured;
			llvm::Value *ErrorV(std::string Str);
			llvm::Value *FatalErrorV(std::string Str);
			llvm::Value *InternalErrorV(std::string Str);
			
			llvm::Type *ErrorType(std::string Str);
			//-----------------------------------------------------------------------------
			//! \return The type corresponding to the given Typename
			//-----------------------------------------------------------------------------
			llvm::Type* getTypeByName(std::string sTypeName);

			//llvm::Function * code_generator::getFunctionFromName( std::string const & name );
			
			//-----------------------------------------------------------------------------
			//! \return The expression created with the shunting yard algorithm.
			//-----------------------------------------------------------------------------
			llvm::Value * code_generator::CreateExpression(	int min_precedence,
															llvm::Value * lhs,
															std::list<ast::operation>::const_iterator& rest_begin,
															std::list<ast::operation>::const_iterator rest_end);
			//-----------------------------------------------------------------------------
			//! \return The value returned from the execution of 'L op R'.
			//-----------------------------------------------------------------------------
			llvm::Value * code_generator::CreateBinaryOperation(llvm::Value * L, llvm::Value * R, token_ids::type op);

		public:
			llvm::Value * operator()(long double const & x);
			llvm::Value * operator()(unsigned int const & x);
			llvm::Value * operator()(int const & x);
			llvm::Value * operator()(bool const & x);
			llvm::Value * operator()(ast::primary_expr const& x);
			llvm::Value * operator()(ast::identifier const & x);
			llvm::Value * operator()(ast::unary_expr const & x);
			llvm::Value * operator()(ast::function_call const & x);
			llvm::Value * operator()(ast::expression const & x);
			//llvm::Value * operator()(ast::operand const & x);
			llvm::Value * operator()(ast::assignment const & x);
			llvm::Value * operator()(ast::variable_declaration const & x);
			llvm::Value * operator()(ast::variable_definition const & x);
			llvm::Value * operator()(ast::statement_list const & x);
			llvm::Value * operator()(ast::statement const & x);
			llvm::Value * operator()(ast::if_statement const & x);
			//llvm::Value * operator()(ast::while_statement const & x);
			//llvm::Value * operator()(ast::return_statement const & x);
			llvm::Function * operator()(ast::function_declaration const & x);
			llvm::Function * operator()(ast::function_definition const & x);

			void print_assembler() const;

		private:
			llvm::LLVMContext & context;
			llvm::IRBuilder<> builder;

			std::unique_ptr<llvm::Module> module;
			
			class VarData
			{
			public:
				VarData(std::string identifier, llvm::AllocaInst* pMem, bool bMutableQualifier)
					: m_identifier(identifier),
					m_pMem(pMem),
					m_bMutableQualifier(bMutableQualifier)
				{
					if(m_identifier.empty())
					{
						throw std::invalid_argument("Empty identifier is invalid for a variable!");
					}
				}
				std::string const & getIdentifier() const {return m_identifier;}
				llvm::AllocaInst* getAllocaInst() const {return m_pMem;}
				bool isMutable() const {return m_bMutableQualifier;}

			private:
				std::string m_identifier;
				llvm::AllocaInst* m_pMem;
				bool m_bMutableQualifier;
			};

			std::vector<llvm::Value*> retValues;
			std::vector<VarData> vLocalSymbolTable;
			std::vector<VarData> vGlobalSymbolTable;

			VarData * getVarFromName( std::string const & name );
		};
	}
}