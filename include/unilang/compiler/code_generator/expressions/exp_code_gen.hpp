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
	// forward declarations
	namespace ast
	{
		struct primary_expr;
		struct identifier;
		struct unary_expr;
		struct function_call;
		struct expression;
		//struct operand;
		struct assignment;
		struct variable_definition;
		struct operation;
	}
	namespace operators
	{
		enum class EOperators : size_t;
	}

	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		// forward declarations
		class code_generator_errors;
		class namespace_code_generator;
		class llvm_code_generator;
		class constants_code_generator;
		class symbol_code_generator;
		class allocation_code_generator;

		//#########################################################################
		//! 
		//#########################################################################
		class expression_code_generator :	//public boost::static_visitor<llvm::Value*>,
											virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			expression_code_generator(	code_generator_errors & codeGeneratorErrors,
										namespace_code_generator & namespaceCodeGenerator,
										llvm_code_generator & llvmCodeGenerator,
										constants_code_generator & constantsCodeGenerator,
										symbol_code_generator & symbolCodeGenerator,
										allocation_code_generator & allocationCodeGenerator);

			typedef llvm::Value * result_type;

			llvm::Value * operator()(ast::primary_expr const& x);
			llvm::Value * operator()(ast::identifier const & x);
			llvm::Value * operator()(ast::unary_expr const & x);
			llvm::Value * operator()(ast::function_call const & x);
			llvm::Value * operator()(ast::expression const & x);
			//llvm::Value * operator()(ast::operand const & x);
			llvm::Value * operator()(ast::assignment const & x);	// TODO: assignment is no expression

			llvm::Value * operator()(ast::variable_definition const & x);

		public:	// public for static_visitor
			llvm::Value * operator()(long double const & x);
			llvm::Value * operator()(double const & x);
			llvm::Value * operator()(float const & x);
			llvm::Value * operator()(uint64_t const & x);
			llvm::Value * operator()(int64_t const & x);
			llvm::Value * operator()(uint32_t const & x);
			llvm::Value * operator()(int32_t const & x);
			llvm::Value * operator()(bool const & x);

		private:
			//-----------------------------------------------------------------------------
			//! \return The expression created with the shunting yard algorithm.
			//-----------------------------------------------------------------------------
			llvm::Value * CreateExpression(	size_t min_precedence,
											llvm::Value * lhs,
											std::vector<ast::operation>::const_iterator& rest_begin,
											std::vector<ast::operation>::const_iterator rest_end);
			//-----------------------------------------------------------------------------
			//! \return The value returned from the execution of 'L op R'.
			//-----------------------------------------------------------------------------
			llvm::Value * CreateBinaryOperation(llvm::Value * L, llvm::Value * R, operators::EOperators op);

		private:
			code_generator_errors & m_codeGeneratorErrors;
			namespace_code_generator & m_namespaceCodeGenerator;
			llvm_code_generator & m_llvmCodeGenerator;
			constants_code_generator & m_constantsCodeGenerator;
			symbol_code_generator & m_symbolCodeGenerator;
			allocation_code_generator & m_allocationCodeGenerator;
		};
	}
}