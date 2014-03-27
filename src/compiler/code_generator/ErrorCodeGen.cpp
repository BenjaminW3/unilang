#include <unilang/compiler/code_generator/ErrorCodeGen.hpp>

#include <unilang/compiler/log/log.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CErrorCodeGen::CErrorCodeGen()
			:m_bErrorOccured(false)
		{
			LOG_SCOPE_DEBUG;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CErrorCodeGen::getErrorLevelString( EErrorLevel err )
		{
			switch (err)
			{
			case EErrorLevel::Standard:	return "ERROR";
			case EErrorLevel::Fatal:	return "FATAL ERROR";
			case EErrorLevel::Internal:	return "INTERNAL ERROR";
			default:					return "UNKNOWN ERROR";
			}
		}
		
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CErrorCodeGen::getErrorOccured() const
		{
			return m_bErrorOccured;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::AllocaInst *CErrorCodeGen::getErrorAllocaInst(std::string Str, EErrorLevel err ) const
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" ALLOCATION: "+Str);
			return nullptr; 
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Value *CErrorCodeGen::getErrorValue(std::string Str, EErrorLevel err ) const
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" VALUE: "+Str); 
			return nullptr; 
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Type *CErrorCodeGen::getErrorType(std::string Str, EErrorLevel err ) const
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" TYPE: "+Str);
			return nullptr; 
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::Function *CErrorCodeGen::getErrorFunction(std::string Str, EErrorLevel err ) const
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+" FUNCTION: "+Str);
			return nullptr; 
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CErrorCodeGen::ErrorBool(std::string Str, EErrorLevel err ) const
		{
			m_bErrorOccured = true;
			LOG(getErrorLevelString(err)+": "+Str);
			return false;
		}
	}
}