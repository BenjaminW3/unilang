#pragma once

//#include "../ast/ast.hpp"

//#include <boost/variant/apply_visitor.hpp>

// forward declarations
namespace llvm
{
	class Function;
}
#include <vector>
#include <map>
#include <memory>
#include <boost/noncopyable.hpp>

#include "symbols/symbols.hpp"
#include "llvm/llvm_code_gen.hpp"
#include "expressions/exp_code_gen.hpp"
#include "allocation/alloc_code_gen.hpp"
#include "statements/statement_code_gen.hpp"

namespace unilang 
{ 
	// forward declarations
	namespace ast
	{
		struct function_declaration;
		struct function_definition;
		struct module;
	}

	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! Generates the code from an AST.
		//!
		//! Given a ast::module it recursively creates the corresponding llvm::Module.
		//#########################################################################
		class code_generator :	public virtual llvm_code_generator, 
								public virtual symbol_code_generator,
								public virtual expression_code_generator,
								public virtual allocation_code_generator,
								public virtual statement_code_generator,
								//public boost::static_visitor<llvm::Function * >,
								virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			code_generator(ast::module const & AST);

		private:
			//-------------------------------------------------------------------------
			//! empty copy constructor
			//-------------------------------------------------------------------------
			//code_generator(code_generator const & cg) {};
			
			//-----------------------------------------------------------------------------
			//! empty assignment-operator
			//-----------------------------------------------------------------------------
			code_generator & operator=( const code_generator & /*cg*/ ) {}
			
		private:
			void addStringConverters();

			
		public:
			typedef llvm::Function * result_type;

			llvm::Function * operator()(ast::function_declaration const & x);
			llvm::Function * operator()(ast::function_definition const & x);
			
			template<typename R, typename T, typename... Args>
			llvm::Function *declare_extern(std::string name, R (*fp)(Args... args))
			{
				std::vector<std::string> lParameterTypeNames; 
				assembleCTypeToUnilangTypeName(lParameterTypeNames, args...);
				std::vector<std::string> lReturnTypeNames;
				assembleCTypeToUnilangTypeName(lReturnTypeNames, cTypeToUnilangTypeName<R>())
				return declare_extern(name, lReturnTypeNames, lParameterTypeNames, false, fp);
			}
			llvm::Function *declare_extern(std::string name, std::vector<std::string> resTypes, std::vector<std::string> const & paramTypes, bool _bIsVarArg, void *fp);
		};
	}
}