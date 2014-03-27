#pragma once

#include <string>

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class AllocaInst;
	class Value;
	class Function;
	class Type;
}
namespace unilang
{
	namespace ast
	{
		struct SVariableDeclaration;
		struct SVariableDefinition;
	}

	namespace code_generator
	{
		class CErrorCodeGen;
		class CLLVMCodeGen;
		class CSymbolCodeGen;
		class CExpressionCodeGen;
	}
}

namespace unilang
{ 
	namespace code_generator
	{
		//#########################################################################
		//! 
		//#########################################################################
		class CAllocationCodeGen :
			//public boost::static_visitor<llvm::AllocaInst*>,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CAllocationCodeGen(
				CErrorCodeGen & codeGeneratorErrors,
				CLLVMCodeGen & llvmCodeGenerator,
				CSymbolCodeGen & symbolCodeGenerator,
				CExpressionCodeGen & expressionCodeGenerator );

			typedef llvm::AllocaInst * result_type;

			llvm::AllocaInst * operator()(ast::SVariableDeclaration const & x);
			llvm::AllocaInst * operator()(ast::SVariableDefinition const & x);

		private:
			//-------------------------------------------------------------------------
			//! Create an alloca instruction in the entry block of the function.
			//! This is used for mutable variables etc.
			//! Only used internally.
			//-------------------------------------------------------------------------
			static llvm::AllocaInst * createEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & sVarName);

			//-------------------------------------------------------------------------
			//! Tries to cast the given value to the given type.
			//! Implemented casts: fXX -> iXX, iXX -> fXX
			//! \return The casted value. 0 if cast was not possible.
			//-------------------------------------------------------------------------
			llvm::Value * createCast(llvm::Value * pVal, llvm::Type * const pDestinationType);

		private:
			CErrorCodeGen & m_codeGeneratorErrors;
			CLLVMCodeGen & m_llvmCodeGenerator;
			CSymbolCodeGen & m_symbolCodeGenerator;
			CExpressionCodeGen & m_expressionCodeGenerator;
		};
	}
}