#include "code_generator.hpp"

#include "../log/log.hpp"

namespace unilang
{ 
	namespace code_generator
	{
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		void code_generator::addStringConverters()
		{
			/*std::vector<llvm::Type*> Doubles(	x._lArgumentExpressions.size(),	llvm::Type::getDoubleTy(context));
			llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::get(context),	Doubles, false);

			llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, x._identifier._name, module.get());

			llvm::Constant *double_to_string_func = module->getOrInsertFunction("double_to_string",
												llvm::Type::getVoidTy(),
												llvm::PointerType::getUnqual(llvm::Type::Int8Ty),
												llvm::Type::Int32Ty,
												llvm::Type::Int32Ty,
												nullptr);

			 llvm::Value* v = llvm::ConstantArray::get(llvmContext, myString.c_str());*/
		}
		
		//-----------------------------------------------------------------------------
		//
		//-----------------------------------------------------------------------------
		code_generator::code_generator(ast::module const & AST)
			:allocation_code_generator(*this)
		{
			LOG_SCOPE_DEBUG;
			
			std::cout << std::endl << "##########CodeGen###########" << std::endl;
			for(ast::meta_entity const & meta : AST._metaEntities)
			{
				//boost::apply_visitor(*this, meta);
				meta.apply_visitor(*this);
			}

			if(m_bErrorOccured)
			{
				throw std::runtime_error("Error occured during compilation of module!");
			}
			std::cout << "############################" << std::endl << std::endl;
			
			// throws excpetion if there are errors
			verify_module();
		}
	}
}