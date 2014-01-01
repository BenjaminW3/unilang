#pragma once

// base classes
#include <boost/noncopyable.hpp>

namespace unilang
{ 
	// forward declarations
	namespace ast
	{
		struct statement;
		struct if_statement;
		struct assignment;
		struct expression;
		struct statement_vector;
		//struct while_statement;
		//struct return_statement;
	}

	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		// forward declarations
		class code_generator_errors;
		class llvm_code_generator;
		class expression_code_generator;

		//#########################################################################
		//! 
		//#########################################################################
		class statement_code_generator :
			//public boost::static_visitor<bool>,
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			statement_code_generator(	code_generator_errors & codeGeneratorErrors,
										llvm_code_generator & llvmCodeGenerator,
										expression_code_generator & expressionCodeGenerator );

		public:
			typedef bool result_type;

			bool operator()(ast::statement const & x);
			bool operator()(ast::if_statement const & x);
			//bool operator()(ast::while_statement const & x);
			//bool operator()(ast::return_statement const & x);
			bool operator()(ast::assignment const & x);
			bool operator()(ast::expression const & x);
			bool operator()(ast::statement_vector const & x);

		private:
			code_generator_errors & m_codeGeneratorErrors;
			llvm_code_generator & m_llvmCodeGenerator;
			expression_code_generator & m_expressionCodeGenerator;
		};
	}
}