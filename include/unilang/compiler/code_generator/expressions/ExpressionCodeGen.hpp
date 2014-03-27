#pragma once

#include <stdint.h>

#include <vector>

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class Value;
}
namespace unilang
{
	namespace lexer
	{
		struct SFloatLiteral;
		struct SIntLiteral;
	}
	namespace ast
	{
		struct SPrimaryExpr;
		struct SIdentifier;
		struct SUnaryExpr;
		struct SFunctionCall;
		struct SExpression;
		//struct SOperand;
		struct SAssignment;
		struct SVariableDefinition;
		struct SOperation;
	}
	namespace operators
	{
		enum class EOperators : size_t;
	}
	namespace code_generator
	{
		class CErrorCodeGen;
		class CModuleCodeGen;
		class CLLVMCodeGen;
		class CConstantsCodeGen;
		class CSymbolCodeGen;
		class CAllocationCodeGen;
	}
}

namespace unilang
{
	//-------------------------------------------------------------------------
	//! The namespace defining the CCodeGenerator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! 
		//#########################################################################
		class CExpressionCodeGen :
			//public boost::static_visitor<llvm::Value*>,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CExpressionCodeGen(	CErrorCodeGen & codeGeneratorErrors,
								CModuleCodeGen & namespaceCodeGenerator,
								CLLVMCodeGen & llvmCodeGenerator,
								CConstantsCodeGen & constantsCodeGenerator,
								CSymbolCodeGen & symbolCodeGenerator,
								CAllocationCodeGen & allocationCodeGenerator);

			typedef llvm::Value * result_type;

			llvm::Value * operator()(ast::SPrimaryExpr const & x);
			llvm::Value * operator()(ast::SIdentifier const & x);
			llvm::Value * operator()(ast::SUnaryExpr const & x);
			llvm::Value * operator()(ast::SFunctionCall const & x);
			llvm::Value * operator()(ast::SExpression const & x);
			//llvm::Value * operator()(ast::SOperand const & x);
			llvm::Value * operator()(ast::SAssignment const & x);	// TODO: assignment is no expression

			llvm::Value * operator()(ast::SVariableDefinition const & x);

		public:	// public for static_visitor
			llvm::Value * operator()(std::string const & x);
			llvm::Value * operator()(lexer::SFloatLiteral const & x);
			llvm::Value * operator()(lexer::SIntLiteral const & x);
			llvm::Value * operator()(bool const & x);

		private:
			//-------------------------------------------------------------------------
			//! \return The expression created with the shunting yard algorithm.
			//-------------------------------------------------------------------------
			llvm::Value * createExpression(
				size_t min_precedence,
				llvm::Value * lhs,
				std::vector<ast::SOperation>::const_iterator& rest_begin,
				std::vector<ast::SOperation>::const_iterator rest_end);
			//-------------------------------------------------------------------------
			//! \return The value returned from the execution of 'L op R'.
			//-------------------------------------------------------------------------
			llvm::Value * createBinaryOperation(llvm::Value * L, llvm::Value * R, operators::EOperators op);

		private:
			CErrorCodeGen & m_codeGeneratorErrors;
			CModuleCodeGen & m_namespaceCodeGenerator;
			CLLVMCodeGen & m_llvmCodeGenerator;
			CConstantsCodeGen & m_constantsCodeGenerator;
			CSymbolCodeGen & m_symbolCodeGenerator;
			CAllocationCodeGen & m_allocationCodeGenerator;
		};
	}
}