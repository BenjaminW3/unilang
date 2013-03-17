#pragma once

#include <string>
#include <cstdint>

#include <vector>

// forward declarations
namespace llvm
{
	class Type;
}

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//! \return The unilang type corresponding to the given c type.
		//-----------------------------------------------------------------------------
		template<typename TYPE>
		std::string cTypeToUnilangTypeName()
		{
			return "ERROR: Unable to translate C type '"+typeid(TYPE).name()+"' to unilang type.";
		}
		template<>
		std::string cTypeToUnilangTypeName<long double>();
		template<>
		std::string cTypeToUnilangTypeName<double>();
		template<>
		std::string cTypeToUnilangTypeName<float>();
		template<>
		std::string cTypeToUnilangTypeName<int64_t>();
		template<>
		std::string cTypeToUnilangTypeName<int32_t>();
		template<>
		std::string cTypeToUnilangTypeName<char>();
		template<>
		std::string cTypeToUnilangTypeName<bool>();
		
		//-----------------------------------------------------------------------------
		//! \return The llvm type converted to a string.
		//-----------------------------------------------------------------------------
		std::string getLLVMTypeName(llvm::Type const * const pType);

		//-----------------------------------------------------------------------------
		//! \return The unilang type corresponding to the given llvm type.
		//-----------------------------------------------------------------------------
		std::string llvmTypeToUnilangTypeName(llvm::Type const * const type);
		
		//-----------------------------------------------------------------------------
		//!
		//-----------------------------------------------------------------------------
		template<typename TYPE>
		void assembleCTypeToUnilangTypeName(std::vector<std::string> & lTypeNames, TYPE const & t)
		{
			lTypeNames.push_back(cTypeToUnilangTypeName<TYPE>());
		}

		template<typename TYPE, typename... Args>
		void assembleCTypeToUnilangTypeName(std::vector<std::string> & lTypeNames, TYPE const & , Args... args)
		{
			std::vector<std::string> lTypeNames;
			lTypeNames.push_back(cTypeToUnilangTypeName<TYPE>());
			assembleCTypeToUnilangTypeName(lTypeNames, args...);
			return lTypeNames;
		}
	}
}