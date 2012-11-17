#pragma once

#include "../ast/ast.hpp"

#include <boost/variant/apply_visitor.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
//#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4244)		// conversion from 'int' to 'unsigned short', possible loss of data
#pragma warning(disable: 4245)		// conversion from 'int' to 'unsigned int', signed/unsigned mismatch
#pragma warning(disable: 4512)		// assignment operator could not be generated
#pragma warning(disable: 4800)		// 'unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#endif

#include "llvm/LLVMContext.h"
#include "llvm/IRBuilder.h"

// predefinitions
namespace llvm
{
	class Module;
	class Value;
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <vector>
#include <map>
#include <memory>
#include <boost/noncopyable.hpp>

namespace unilang 
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		unsigned int const uiIntSize = 64;

		//#########################################################################
		//! The code_generator internal class holding data about variables.
		//#########################################################################
		class VarData
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
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
		
		//#########################################################################
		//! 
		//#########################################################################
		class llvm_code_generator : virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			llvm_code_generator();

			//-------------------------------------------------------------------------
			//! Optimizes the bytecode.
			//-------------------------------------------------------------------------
			void optimize() const;
			
			//-------------------------------------------------------------------------
			//! Prints out the bytecode.
			//-------------------------------------------------------------------------
			void print_bytecode() const;

			//-------------------------------------------------------------------------
			//! \return The generated module.
			//-------------------------------------------------------------------------
			std::shared_ptr<llvm::Module> getModule() const;
			//-------------------------------------------------------------------------
			//! \return The LLVMContext used to build the module.
			//-------------------------------------------------------------------------
			llvm::LLVMContext& getContext() const;
			//-------------------------------------------------------------------------
			//! \return The IRBuilder used to build the module.
			//-------------------------------------------------------------------------
			llvm::IRBuilder<>& getBuilder();

		protected:
			llvm::LLVMContext & context;
			llvm::IRBuilder<> builder;

			std::shared_ptr<llvm::Module> module;
		};

		//#########################################################################
		//! 
		//#########################################################################
		class base_code_generator : public virtual llvm_code_generator,
									virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			base_code_generator();

		protected:
			//-----------------------------------------------------------------------------
			//! \return Is Set each of the (Fatal|Internal)Error(V|Type) methods when called.
			//-----------------------------------------------------------------------------
			bool m_bErrorOccured;
		
			//-----------------------------------------------------------------------------
			//! Standard: An invalid value for use in standard error situations.
			//! Fatal: An invalid value for use in fatal error situations.
			//! Internal: An invalid value for use in internal error situations of the compiler independant of the validity of the compilation.
			//-----------------------------------------------------------------------------
			enum class EErrorLevel
			{
				Standard,
				Fatal,
				Internal,
			};
			//-----------------------------------------------------------------------------
			//! \return String corresponding to the error level.
			//-----------------------------------------------------------------------------
			std::string getErrorLevelString( EErrorLevel err );

			//-----------------------------------------------------------------------------
			//! \return An invalid value for use in error situations.
			//-----------------------------------------------------------------------------
			llvm::Value *ErrorValue(std::string str, EErrorLevel err = EErrorLevel::Standard );
			
			//-----------------------------------------------------------------------------
			//! \return An invalid type for type errors.
			//-----------------------------------------------------------------------------
			llvm::Type *ErrorType(std::string Str, EErrorLevel err = EErrorLevel::Standard );

			//-----------------------------------------------------------------------------
			//! \return An invalid function for errors.
			//-----------------------------------------------------------------------------
			llvm::Function *ErrorFunction(std::string Str, EErrorLevel err = EErrorLevel::Standard );

			
			//-----------------------------------------------------------------------------
			//! \return The type corresponding to the given Typename
			//-----------------------------------------------------------------------------
			llvm::Type* getTypeByName(std::string sTypeName);
			
			//-----------------------------------------------------------------------------
			//! \return The Function with the given name.
			//-----------------------------------------------------------------------------
			llvm::Function * getFunctionFromName( std::string const & name );
			
			//-----------------------------------------------------------------------------
			//! \return The Variable with the given name in the current scope.
			//-----------------------------------------------------------------------------
			VarData const * const getVarFromName( std::string const & name );

			std::vector<VarData> vLocalSymbolTable;
			std::vector<VarData> vGlobalSymbolTable;

		};
		
		//#########################################################################
		//! 
		//#########################################################################
		class expression_code_generator :	public virtual llvm_code_generator,
											public virtual base_code_generator,
											//public boost::static_visitor<llvm::Value*>,
											virtual boost::noncopyable
		{
		protected:
			//-----------------------------------------------------------------------------
			//! \return The expression created with the shunting yard algorithm.
			//-----------------------------------------------------------------------------
			llvm::Value * CreateExpression(	int min_precedence,
											llvm::Value * lhs,
											std::list<ast::operation>::const_iterator& rest_begin,
											std::list<ast::operation>::const_iterator rest_end);
			//-----------------------------------------------------------------------------
			//! \return The value returned from the execution of 'L op R'.
			//-----------------------------------------------------------------------------
			llvm::Value * CreateBinaryOperation(llvm::Value * L, llvm::Value * R, token_ids::type op);

			//-----------------------------------------------------------------------------
			//! Create an alloca instruction in the entry block of the function.  
			//! This is used for mutable variables etc.
			//-----------------------------------------------------------------------------
			static llvm::AllocaInst * CreateEntryBlockAlloca(llvm::Function * const TheFunction, llvm::Type * const pType, std::string const & VarName);

		public:
			typedef llvm::Value * result_type;

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
		};

		//#########################################################################
		//! 
		//#########################################################################
		class statement_code_generator :	public virtual llvm_code_generator,
											public virtual base_code_generator,
											public virtual expression_code_generator,
											//public boost::static_visitor<bool>,
											virtual boost::noncopyable
		{
		protected:
			//-----------------------------------------------------------------------------
			//! \return An error value.
			//-----------------------------------------------------------------------------
			bool ErrorBool(std::string Str, EErrorLevel err = EErrorLevel::Standard );

		public:
			typedef bool result_type;

			bool operator()(ast::statement_list const & x);
			bool operator()(ast::statement const & x);
			bool operator()(ast::expression const & x);
			bool operator()(ast::if_statement const & x);
			//bool operator()(ast::while_statement const & x);
			//bool operator()(ast::return_statement const & x);
		};

		//#########################################################################
		//! Generates the code from an AST.
		//!
		//! Given a ast::module it recursively creates the corresponding llvm::Module.
		//#########################################################################
		class code_generator :	public virtual llvm_code_generator, 
								public virtual base_code_generator,
								public virtual expression_code_generator,
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

		private:
			std::vector<llvm::Value*> retValues;
		};
	}
}