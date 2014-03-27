#include <unilang/compiler/code_generator/module/ModuleCodeGen.hpp>

#include <unilang/compiler/code_generator/CodeGenerator.hpp>

#include <unilang/compiler/ast/AST.hpp>

#include <unilang/compiler/log/log.hpp>

namespace unilang
{ 
	namespace code_generator
	{
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		CModuleCodeGen::CModuleCodeGen(	
			CErrorCodeGen & codeGeneratorErrors, 
			CFunctionCodeGen & functionCodeGenerator ) :
			m_codeGeneratorErrors	(codeGeneratorErrors),
			m_functionCodeGenerator	(functionCodeGenerator)
		{};
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CModuleCodeGen::operator()(ast::SModule const & x)
		{
			LOG_SCOPE_DEBUG;
			
			bool bSuccess (true);
			for(ast::SMetaEntity const & meta : x.m_vMetaEntities)
			{
				bSuccess &= (meta.apply_visitor(*this));
			}

			return bSuccess;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CModuleCodeGen::operator()(ast::SNamespaceDeclaration const & x)
		{
			LOG_SCOPE_DEBUG;
			
			m_vsNamespaceHierarchy.push_back(x.m_idfName.m_sName);
			
			bool bSuccess (true);
			for(ast::SMetaEntity const & meta : x.m_vMetaEntities)
			{
				bSuccess &= (meta.apply_visitor(*this));
			}
			
			m_vsNamespaceHierarchy.pop_back();

			return bSuccess;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CModuleCodeGen::operator()(ast::SFunctionDeclaration const & x)
		{
			return m_functionCodeGenerator(x) != nullptr;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		bool CModuleCodeGen::operator()(ast::SFunctionDefinition const & x)
		{
			return m_functionCodeGenerator(x) != nullptr;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::vector<std::string> CModuleCodeGen::getCurrentNamespaceHierarchy() const
		{
			return m_vsNamespaceHierarchy;
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CModuleCodeGen::getCurrentMangledNamespaceName() const
		{
			return buildMangledNamespaceName(getCurrentNamespaceHierarchy());
		}
		//-------------------------------------------------------------------------
		//
		//-------------------------------------------------------------------------
		std::string CModuleCodeGen::buildMangledNamespaceName( std::vector<std::string> const & vsNamespaceHierarchy )
		{
			std::string sMangledNamespaceName;
			for(std::string const & sNamespace : vsNamespaceHierarchy)
			{
				sMangledNamespaceName += sNamespace + "::";
			}
			return sMangledNamespaceName;
		}
	}
}