#pragma once

// members
#include <string>

// forward declarations
namespace llvm
{
//	class Module;
//	class Value;
	class AllocaInst;
}

namespace unilang
{ 
	//-------------------------------------------------------------------------
	//! The namespace defining the CCodeGenerator.
	//-------------------------------------------------------------------------
	namespace code_generator
	{
		//#########################################################################
		//! The class holding data about variables.
		//#########################################################################
		class VarData
		{
		public:
			//-------------------------------------------------------------------------
			//! Constructor
			//-------------------------------------------------------------------------
			VarData(std::string identifier, llvm::AllocaInst * pMem, bool bMutableQualifier);
			//-------------------------------------------------------------------------
			//! Copy-constructor
			//-------------------------------------------------------------------------
			VarData(VarData const & var);

			std::string const & getIdentifier() const;
			llvm::AllocaInst * getAllocaInst() const;
			bool isMutable() const;

		private:
			std::string m_identifier;
			llvm::AllocaInst * m_pMem;
			bool m_bMutableQualifier;
		};
	}
}