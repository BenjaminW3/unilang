#include "types.hpp"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// assignment operator could not be generated
#endif

#include <llvm/Type.h>
#include <llvm/Support/raw_ostream.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang 
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		template<>
		std::string cTypeToUnilangTypeName<long double>()
		{
			return "f64";
		}
		template<>
		std::string cTypeToUnilangTypeName<double>()
		{
			return "f64";
		}
		template<>
		std::string cTypeToUnilangTypeName<float>()
		{
			return "f32";
		}
		template<>
		std::string cTypeToUnilangTypeName<int64_t>()
		{
			return "i64";
		}
		template<>
		std::string cTypeToUnilangTypeName<int32_t>()
		{
			return "i32";
		}
		template<>
		std::string cTypeToUnilangTypeName<bool>()
		{
			return "i1";
		}
		
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::string getLLVMTypeName(llvm::Type * const pType)
		{
			std::string type_str;
			llvm::raw_string_ostream rso(type_str);
			pType->print(rso);
			return rso.str();
		}
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::string llvmTypeToUnilangTypeName(llvm::Type * type)
		{
			if(type->isDoubleTy())
			{
				return "f64";
			}
			else if(type->isFloatTy())
			{
				return "f32";
			}
			else if(type->isIntegerTy() && type->getScalarSizeInBits() == 32)
			{
				return "i32";
			}
			else if(type->isIntegerTy() && type->getScalarSizeInBits() == 64)
			{
				return "i64";
			}
			else if(type->isIntegerTy() && type->getScalarSizeInBits() == 1)
			{
				return "i1";
			}
			else
			{
				return "ERROR: Unable to translate llvm type '"+getLLVMTypeName(type)+"' to unilang type.";
			}
		}
	}
}