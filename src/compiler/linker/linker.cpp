#include <unilang/compiler/linker/linker.hpp>

#include <unilang/compiler/ast/function_ast.hpp>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4100)		// unreferenced formal parameter
#pragma warning(disable: 4127)		// conditional expression is constant
#pragma warning(disable: 4244)		// conversion from 'uint64_t' to 'const unsigned int', possible loss of data
#pragma warning(disable: 4245)		// 'argument' : conversion from 'llvm::AttrListPtr::AttrIndex' to 'unsigned int'
#pragma warning(disable: 4146)		// unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable: 4267)		// conversion from 'size_t' to 'unsigned int', possible loss of data
#pragma warning(disable: 4512)		// assignment operator could not be generated
#pragma warning(disable: 4800)		// 'unsigned int' : forcing value to bool 'true' or 'false' (performance warning)
#endif

#include <llvm/IR/Module.h>

// Bitcode
#include <llvm/Support/MemoryBuffer.h>	// MemoryBuffer
#include <llvm/Support/system_error.h>
#include <llvm/Bitcode/ReaderWriter.h>	// ParseBitcodeFile
#include <llvm/IR/LLVMContext.h>		// getGlobalContext
#include <llvm/Support/raw_ostream.h>

//Linker
#include <llvm/Linker.h>

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#include <iostream>

namespace unilang
{ 
	namespace linker
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> load_module_from_bitcode_file( std::string const & sBitCodeFilePath )
		{
			// TODO: check file validity

			//std::unique_ptr<llvm::MemoryBuffer> buffer;
			llvm::OwningPtr<llvm::MemoryBuffer> buffer;
			llvm::error_code const error (llvm::MemoryBuffer::getFile(sBitCodeFilePath.c_str(), buffer));
			if(!buffer)
			{
				throw std::runtime_error("Unable to getFile '" + sBitCodeFilePath + " with error: " + error.message());
			}

			std::string sError;
			std::shared_ptr<llvm::Module> module(llvm::ParseBitcodeFile(buffer.get(), llvm::getGlobalContext(), &sError));
			if(!module)
			{
				throw std::runtime_error("Unable to ParseBitcodeFile '" + sBitCodeFilePath + "' with error: " + sError);
			}

			// TODO: This bitcode to module loading process could be done lazily:
			/*// Create the runtime library module provider, which will lazily stream functions out of the module.
			MemoryBuffer *buffer = MemoryBuffer::getFile("mylibc.bc");
			ModuleProvider *LMP = getBitcodeModuleProvider(buffer);
			Module *LM = LMP->getModule();*/

			return module;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::vector<std::shared_ptr<llvm::Module>> load_modules_from_bitcode_files( std::vector<std::string> const & vsBitcodeFilePaths )
		{
			std::vector<std::shared_ptr<llvm::Module>> vspModules;
			
			for(auto const & sBitcodeFilePath : vsBitcodeFilePaths)
			{
				vspModules.push_back(load_module_from_bitcode_file(sBitcodeFilePath));
			}

			return vspModules;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void write_module_to_bitcode_file( llvm::Module const & module, std::string const & sBitCodeFilePath )
		{
			std::string sError;
			llvm::raw_fd_ostream fd_ostream(sBitCodeFilePath.c_str(), sError);
			if(fd_ostream.has_error())
			{
				throw std::runtime_error("Unable to open file '" + sBitCodeFilePath + "' for bitcode writing with error: " + sError);
			}

			llvm::WriteBitcodeToFile(&module, fd_ostream);
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> link_modules( std::vector<std::shared_ptr<llvm::Module>> vModules )
		{
			std::shared_ptr<llvm::Module> newModule (std::make_shared<llvm::Module>("unilang-module", llvm::getGlobalContext()));
			llvm::Linker linker (newModule.get());

			bool bErrorOccured (false);
			for(auto const & module : vModules)
			{
				std::string sError;
				if(linker.linkInModule(module.get(), &sError))
				{
					bErrorOccured = true;
					std::cout << "Linker Error: '" + module->getModuleIdentifier() + "' :" + sError;
				}
				else if(!sError.empty())
				{
					std::cout << "Linker Warning: '" + module->getModuleIdentifier() + "' :" + sError;
				}
			}

			return bErrorOccured ? nullptr : newModule;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> link_bitcode_files_to_module( std::vector<std::string> const & vsBitcodeFilePaths ) 
		{
			return link_modules(load_modules_from_bitcode_files(vsBitcodeFilePaths));
		}
	}
}