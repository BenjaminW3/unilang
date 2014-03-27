#include <unilang/compiler/code_generator/symbols/Variable.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//! Constructor
		//-------------------------------------------------------------------------
		VarData::VarData(std::string identifier, llvm::AllocaInst * pMem, bool bMutableQualifier) :
			m_identifier(identifier),
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
		VarData::VarData(VarData const & var) :
			m_identifier(var.m_identifier),
			m_pMem(var.m_pMem),
			m_bMutableQualifier(var.m_bMutableQualifier)
		{
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