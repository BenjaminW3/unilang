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
	//! The namespace defining the CCodeGenerator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//! Standard: Standard error situations.
		//! Fatal: Fatal error situations.
		//! Internal: Internal error situations of the compiler independant of the validity of the compilation.
		//-------------------------------------------------------------------------
		enum class EErrorLevel
		{
			Standard,
			Fatal,
			Internal,
		};

		//#########################################################################
		//! A factory for error values of different types.
		//! These invalid values are used in case of a code generation error to not abort the compilation on the first error.
		//#########################################################################
		class CErrorCodeGen :
			virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor.
			//-------------------------------------------------------------------------
			CErrorCodeGen();

			//-------------------------------------------------------------------------
			//! \return String corresponding to the error level.
			//-------------------------------------------------------------------------
			static std::string getErrorLevelString( EErrorLevel err );
			
			//-------------------------------------------------------------------------
			//! \return If an error has occured during the code generation.
			//! This flag is set if any of the Error methods is called and can not be reset. 
			//-------------------------------------------------------------------------
			bool getErrorOccured() const;

			//-------------------------------------------------------------------------
			//! \return An error value.
			//-------------------------------------------------------------------------
			bool ErrorBool(std::string Str, EErrorLevel err = EErrorLevel::Standard ) const;

			//-------------------------------------------------------------------------
			//! \return An invalid value for use in error situations.
			//-------------------------------------------------------------------------
			llvm::Value * getErrorValue(std::string str, EErrorLevel err = EErrorLevel::Standard ) const;
			
			//-------------------------------------------------------------------------
			//! \return An invalid type for type errors.
			//-------------------------------------------------------------------------
			llvm::Type * getErrorType(std::string Str, EErrorLevel err = EErrorLevel::Standard ) const;
			
			//-------------------------------------------------------------------------
			//! \return An invalid function for errors.
			//-------------------------------------------------------------------------
			llvm::Function * getErrorFunction(std::string Str, EErrorLevel err = EErrorLevel::Standard ) const;
			
			//-------------------------------------------------------------------------
			//! \return An invalid alloca for use in error situations.
			//-------------------------------------------------------------------------
			llvm::AllocaInst * getErrorAllocaInst(std::string str, EErrorLevel err = EErrorLevel::Standard ) const;

		private:
			//-------------------------------------------------------------------------
			//! \return Is Set by each of the (Fatal|Internal)Error(V|Type) methods when called.
			//-------------------------------------------------------------------------
			mutable bool m_bErrorOccured;
		};
	}
}