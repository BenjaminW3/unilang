#include <unilang/compiler/code_generator/Types.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// assignment operator could not be generated
#pragma warning(disable: 4244)		// 'initializing' : conversion from 'llvm::Twine::NodeKind' to 'unsigned char', possible loss of data
#endif

#include <llvm/IR/Type.h>
#include <llvm/Support/raw_ostream.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
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
		std::string cTypeToUnilangTypeName<int64_t>()	// __int64, long long, signed long long
		{
			return "i64";
		}
		template<>
		std::string cTypeToUnilangTypeName<int32_t>()	// __int32, signed, signed int, int
		{
			return "i32";
		}
		template<>
		std::string cTypeToUnilangTypeName<int16_t>()	// __int16, short, short int, signed short int
		{
			return "i16";
		}
		template<>
		std::string cTypeToUnilangTypeName<char>()
		{
			return "i8";
		}
		template<>
		std::string cTypeToUnilangTypeName<bool>()
		{
			return "i1";
		}
		
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string getLLVMTypeName(llvm::Type const * const pType)
		{
			std::string type_str;
			llvm::raw_string_ostream rso(type_str);
			pType->print(rso);
			return rso.str();
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string llvmTypeToUnilangTypeName(llvm::Type const * const type)
		{
			if(type->isDoubleTy())
			{
				return "f64";
			}
			else if(type->isFloatTy())
			{
				return "f32";
			}
			else if(type->isIntegerTy() && type->getPrimitiveSizeInBits() == 64)	// use getPrimitiveSizeInBits because LLVM is missing const for getScalarSizeInBits
			{
				return "i64";
			}
			else if(type->isIntegerTy() && type->getPrimitiveSizeInBits() == 32)
			{
				return "i32";
			}
			else if(type->isIntegerTy() && type->getPrimitiveSizeInBits() == 16)
			{
				return "i16";
			}
			else if(type->isIntegerTy() && type->getPrimitiveSizeInBits() == 8)
			{
				return "i8";
			}
			else if(type->isIntegerTy() && type->getPrimitiveSizeInBits() == 1)
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