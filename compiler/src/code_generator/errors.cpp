#include "errors.hpp"

#include "../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator_errors::code_generator_errors()
			:m_bErrorOccured(false)
		{
			LOG_SCOPE_DEBUG;
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::string code_generator_errors::getErrorLevelString( EErrorLevel err )
		{
			switch (err)
			{
			case EErrorLevel::Standard: return "ERROR";
			case EErrorLevel::Fatal: return "FATAL ERROR";
			case EErrorLevel::Internal: return "INTERNAL ERROR";
			default: return "UNKNOWN ERROR";
			}
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Value *code_generator_errors::ErrorValue(std::string Str, EErrorLevel err )
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" VALUE: "+Str); 
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Type *code_generator_errors::ErrorType(std::string Str, EErrorLevel err )
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" TYPE: "+Str);
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		llvm::Function *code_generator_errors::ErrorFunction(std::string Str, EErrorLevel err )
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" FUNCTION: "+Str);
			return nullptr; 
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		bool code_generator_errors::ErrorBool(std::string Str, EErrorLevel err )
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+": "+Str);
			return false;
		}
	}
}