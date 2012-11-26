#pragma once

#include <string>

#include <boost/noncopyable.hpp>

// predefinitions
namespace llvm
{
	class Value;
	class Type;
	class Function;
}

namespace unilang 
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! 
		//#########################################################################
		class code_generator_errors :	virtual boost::noncopyable
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			code_generator_errors();

		protected:
			//-----------------------------------------------------------------------------
			//! \return Is Set by each of the (Fatal|Internal)Error(V|Type) methods when called.
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
			//! \return An error value.
			//-----------------------------------------------------------------------------
			bool ErrorBool(std::string Str, EErrorLevel err = EErrorLevel::Standard );

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
		};
	}
}