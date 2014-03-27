#pragma once

// base classes
#include <boost/noncopyable.hpp>

// forward declarations
namespace unilang
{
	namespace ast
	{
		struct SStatement;
		struct SIfStatement;
		struct SAssignment;
		struct SExpression;
		struct SStatementVector;
		//struct SWhileStatement;
		//struct SReturnStatement;
	}

	namespace code_generator
	{
		class CErrorCodeGen;
		class CLLVMCodeGen;
		class CExpressionCodeGen;
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
		class CStatementCodeGen :
			//public boost::static_visitor<bool>,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CStatementCodeGen(	
				CErrorCodeGen & codeGeneratorErrors,
				CLLVMCodeGen & llvmCodeGenerator,
				CExpressionCodeGen & expressionCodeGenerator );

		public:
			typedef bool result_type;

			bool operator()(ast::SStatement const & x);
			bool operator()(ast::SIfStatement const & x);
			//bool operator()(ast::SWhileStatement const & x);
			//bool operator()(ast::SReturnStatement const & x);
			bool operator()(ast::SAssignment const & x);
			bool operator()(ast::SExpression const & x);
			bool operator()(ast::SStatementVector const & x);

		private:
			CErrorCodeGen & m_codeGeneratorErrors;
			CLLVMCodeGen & m_llvmCodeGenerator;
			CExpressionCodeGen & m_expressionCodeGenerator;
		};
	}
}