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
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> compile_file( std::string const & sSourceCodeFilePath );
	}
}