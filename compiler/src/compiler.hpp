#pragma once

#include <string>
#include <memory>

// predefinitions
namespace llvm
{
	class Module;
}

namespace unilang
{
	namespace compiler
	{
		enum EDebugOutputOptions
		{
			Standard	= 0,
			SourceCode	= 1,
			Unoptimized	= 2,
			Optimized	= 4,
			All			= SourceCode | Unoptimized | Optimized,
		};
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> compile_file( std::string const & sSourceCodeFilePath, EDebugOutputOptions const output = EDebugOutputOptions::Standard );
	}
}