#include "symbols.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//! Constructor
		//-------------------------------------------------------------------------
		VarData::VarData(std::string identifier, llvm::AllocaInst* pMem, bool bMutableQualifier)
			: m_identifier(identifier),
			m_pMem(pMem),
			m_bMutableQualifier(bMutableQualifier)
		{
			if(m_identifier.empty())
			{
				throw std::invalid_argument("Empty identifier is invalid for a variable!");
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string const & VarData::getIdentifier() const 
		{
			return m_identifier;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		llvm::AllocaInst* VarData::getAllocaInst() const 
		{
			return m_pMem;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool VarData::isMutable() const 
		{
			return m_bMutableQualifier;
		}
	}
}