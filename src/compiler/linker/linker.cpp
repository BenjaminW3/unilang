#include <unilang/compiler/linker/Linker.hpp>

#include <unilang/compiler/ast/Function.hpp>

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
#include <llvm/IRReader/IRReader.h>		// ParseIRFile
#include <llvm/Support/SourceMgr.h>		// SMDiagnostic
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
		std::shared_ptr<llvm::Module> loadModuleFromBitcodeFile( std::string const & sBitCodeFilePath )
		{
			// TODO: check file validity

			llvm::OwningPtr<llvm::MemoryBuffer> buffer;
			llvm::error_code const error (llvm::MemoryBuffer::getFile(sBitCodeFilePath.c_str(), buffer));
			if(!buffer)
			{
				throw std::runtime_error("Unable to getFile '" + sBitCodeFilePath + " with error: " + error.message() + ".");
			}

			llvm::LLVMContext & Context(llvm::getGlobalContext());

			llvm::ErrorOr<llvm::Module *> moduleOrError(llvm::getLazyBitcodeModule(buffer.get(), Context));
			if(!moduleOrError)
			{
				throw std::runtime_error("Unable to getLazyBitcodeModule '" + sBitCodeFilePath + "' with error: " + moduleOrError.getError().message() + ".");
			}
			else
			{
				// On success getLazyBitcodeModule takes ownership of the buffer.
				buffer.take();
				return std::shared_ptr<llvm::Module>(moduleOrError.get());
			}
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::vector<std::shared_ptr<llvm::Module>> loadModulesFromBitcodeFiles( std::vector<std::string> const & vsBitcodeFilePaths )
		{
			std::vector<std::shared_ptr<llvm::Module>> vspModules;
			
			for(auto const & sBitcodeFilePath : vsBitcodeFilePaths)
			{
				auto const spModule (loadModuleFromBitcodeFile(sBitcodeFilePath));
				if(spModule)
				{
					vspModules.push_back(spModule);
				}
				else
				{
					throw std::runtime_error("Invalid module returned from loadModuleFromBitcodeFile for file '" + sBitcodeFilePath + "'.");
				}
			}

			return vspModules;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		void writeModuleToBitcodeFile( llvm::Module const & module, std::string const & sBitCodeFilePath )
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
		std::shared_ptr<llvm::Module> loadModuleFromIRFile(std::string const & sIRCodeFilePath)
		{
			// TODO: check file validity

			llvm::LLVMContext & Context (llvm::getGlobalContext());

			llvm::SMDiagnostic Err;
			std::shared_ptr<llvm::Module> const spModule(llvm::getLazyIRFileModule(sIRCodeFilePath, Err, Context));
			if(!spModule)
			{
				std::string sError;
				llvm::raw_string_ostream rso(sError);
				Err.print(nullptr, rso);	// nullptr == ProgName
				throw std::runtime_error("Unable to ParseIRFile '" + sIRCodeFilePath + " with error: " + rso.str() + ".");
			}

			return spModule;
		}

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		/*for(unsigned i = 0, e = ExtraObjects.size(); i != e; ++i)
		{
			ErrorOr<object::ObjectFile *> Obj =
				object::ObjectFile::createObjectFile(ExtraObjects[i]);
			if(!Obj)
			{
				Err.print(argv[0], errs());
				return 1;
			}
			EE->addObjectFile(Obj.get());
		}

		for(unsigned i = 0, e = ExtraArchives.size(); i != e; ++i)
		{
			OwningPtr<MemoryBuffer> ArBuf;
			error_code ec;
			ec = MemoryBuffer::getFileOrSTDIN(ExtraArchives[i], ArBuf);
			if(ec)
			{
				Err.print(argv[0], errs());
				return 1;
			}
			object::Archive *Ar = new object::Archive(ArBuf.take(), ec);
			if(ec || !Ar)
			{
				Err.print(argv[0], errs());
				return 1;
			}
			EE->addArchive(Ar);
		}*/

		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::shared_ptr<llvm::Module> linkModules( std::vector<std::shared_ptr<llvm::Module>> vModules )
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
		std::shared_ptr<llvm::Module> linkBitcodeFilesToModule( std::vector<std::string> const & vsBitcodeFilePaths ) 
		{
			return linkModules(loadModulesFromBitcodeFiles(vsBitcodeFilePaths));
		}
	}
}