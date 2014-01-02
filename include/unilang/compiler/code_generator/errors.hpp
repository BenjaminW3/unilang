#pragma once

#include <string>

#include <boost/noncopyable.hpp>

// forward declarations
namespace llvm
{
	class Value;
	class Type;
	class Function;
	class AllocaInst;
}

namespace unilang
{ 
	//-------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//! Standard: An invalid value for use in standard error situations.
		//! Fatal: An invalid value for use in fatal error situations.
		//! Internal: An invalid value for use in internal error situations of the compiler independant of the validity of the compilation.
		//-------------------------------------------------------------------------
		enum class EErrorLevel
		{
			Standard,
			Fatal,
			Internal,
		};

		//#########################################################################
		//! 
		//#########################################################################
		class code_generator_errors :
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			code_generator_errors();

			//-------------------------------------------------------------------------
			//! \return String corresponding to the error level.
			//-------------------------------------------------------------------------
			static std::string getErrorLevelString( EErrorLevel err );
			
			//-------------------------------------------------------------------------
			//! \return If an error has occured during the code generation.
			//! This flag can not be reset. 
			//-------------------------------------------------------------------------
			bool getErrorOccured() const;

			//-------------------------------------------------------------------------
			//! \return An error value.
			//-------------------------------------------------------------------------
			bool ErrorBool(std::string Str, EErrorLevel err = EErrorLevel::Standard ) const;

			//-------------------------------------------------------------------------
			//! \return An invalid value for use in error situations.
			//-------------------------------------------------------------------------
			llvm::Value *ErrorValue(std::string str, EErrorLevel err = EErrorLevel::Standard ) const;
			
			//-------------------------------------------------------------------------
			//! \return An invalid type for type errors.
			//-------------------------------------------------------------------------
			llvm::Type *ErrorType(std::string Str, EErrorLevel err = EErrorLevel::Standard ) const;
			
			//-------------------------------------------------------------------------
			//! \return An invalid function for errors.
			//-------------------------------------------------------------------------
			llvm::Function *ErrorFunction(std::string Str, EErrorLevel err = EErrorLevel::Standard ) const;
			
			//-------------------------------------------------------------------------
			//! \return An invalid alloca for use in error situations.
			//-------------------------------------------------------------------------
			llvm::AllocaInst *ErrorAllocaInst(std::string str, EErrorLevel err = EErrorLevel::Standard ) const;

		private:
			//-------------------------------------------------------------------------
			//! \return Is Set by each of the (Fatal|Internal)Error(V|Type) methods when called.
			//-------------------------------------------------------------------------
			mutable bool m_bErrorOccured;
		};
	}
}