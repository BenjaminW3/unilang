#pragma once

#include <string>

// predefinitions
namespace llvm
{
	class Module;
	class Value;
	class AllocaInst;
}
//#include <boost/noncopyable.hpp>

namespace unilang 
{ 
	//-----------------------------------------------------------------------------
	//! The namespace defining the code_generator.
	//-----------------------------------------------------------------------------
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
			VarData(std::string identifier, llvm::AllocaInst* pMem, bool bMutableQualifier);

			std::string const & getIdentifier() const;
			llvm::AllocaInst* getAllocaInst() const;
			bool isMutable() const;

		private:
			std::string m_identifier;
			llvm::AllocaInst* m_pMem;
			bool m_bMutableQualifier;
		};
	}
}