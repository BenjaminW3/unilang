#pragma once

#include <vector>
#include <unilang/compiler/ast/Base.hpp>
#include <type_traits>

namespace unilang
{
	//#########################################################################
	//! The annotaion handler links the AST to a map of iterator positions for the purpose of subsequent semantic error handling when the program is being compiled.
	//#########################################################################
	template <typename TLexerTokenIterator>
	struct SAnnotationHandler
	{
	public:
		template <typename, typename>
		struct result { typedef void type; };

		std::vector<TLexerTokenIterator> & m_iters;
		
		//-------------------------------------------------------------------------
		//! Constructor.
		//-------------------------------------------------------------------------
		SAnnotationHandler(std::vector<TLexerTokenIterator>& iters) :
			m_iters(iters)
		{}
		
		//-------------------------------------------------------------------------
		//! Copy-constructor.
		//-------------------------------------------------------------------------
		SAnnotationHandler(SAnnotationHandler const & an) :
			m_iters(an.m_iters)
		{}
		//-------------------------------------------------------------------------
		//! Assignment-operator.
		//-------------------------------------------------------------------------
		SAnnotationHandler & operator=(const SAnnotationHandler & an)
		{
			m_iters = an.m_iters;
		}
		
		//-------------------------------------------------------------------------
		//! Sets ID if its base is ast::SASTBase.
		//-------------------------------------------------------------------------
		template <typename T>
		typename std::enable_if<std::is_base_of<ast::SASTBase, T>::value, void>::type  operator()(T& ast, TLexerTokenIterator pos) const
		{
			ast.m_uiID = m_iters.size();
			m_iters.push_back(pos);
		}
		//-------------------------------------------------------------------------
		//! Can not set ID if its base is not ast::SASTBase.
		//-------------------------------------------------------------------------
		template <typename T>
		typename std::enable_if<!std::is_base_of<ast::SASTBase, T>::value, void>::type  operator()(T& /*ast*/, TLexerTokenIterator /*pos*/) const
		{
			std::cout << "NOT ABLE TO SET ANNOTATION ID FOR: " << typeid(T).name();
		}
	};
}