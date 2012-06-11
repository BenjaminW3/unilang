#pragma once

#include <map>
#include <boost/variant/apply_visitor.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/mpl/bool.hpp>
#include "../ast/ast.hpp"

namespace unilang
{
    ///////////////////////////////////////////////////////////////////////////////
    //  The annotation handler links the AST to a map of iterator positions
    //  for the purpose of subsequent semantic error handling when the
    //  program is being compiled.
    ///////////////////////////////////////////////////////////////////////////////
/*    template <typename Iterator>
    struct annotation
    {
        template <typename, typename>
        struct result { typedef void type; };

        std::vector<Iterator>& iters;
        annotation(std::vector<Iterator>& iters)
          : iters(iters) {}

        struct set_id
        {
            typedef void result_type;

            int id;
            set_id(int id) : id(id) {}

            void operator()(ast::function_call& x) const
            {
                x.idf.id = id;
            }

            void operator()(ast::identifier& x) const
            {
                x.id = id;
            }
#if defined(_MSC_VER)
#pragma warning(disable: 4100)	// warning C4100: 'x' : unreferenced formal parameter
#endif
            template <typename T>
            void operator()(T& x) const
            {
                // no-op
            }
        };
#if defined(_MSC_VER)
#pragma warning(default: 4100)
#endif
        void operator()(ast::operand& ast, Iterator pos) const
        {
            int id = iters.size();
            iters.push_back(pos);
            boost::apply_visitor(set_id(id), ast);
        }
        void operator()(ast::variable_declaration& ast, Iterator pos) const
        {
            int id = iters.size();
            iters.push_back(pos);
            ast.lhs.id = id;
        }

        void operator()(ast::assignment& ast, Iterator pos) const
        {
            int id = iters.size();
            iters.push_back(pos);
            ast.lhs.id = id;
        }

        void operator()(ast::return_statement& ast, Iterator pos) const
        {
            int id = iters.size();
            iters.push_back(pos);
            ast.id = id;
        }

        void operator()(ast::identifier& ast, Iterator pos) const
        {
            int id = iters.size();
            iters.push_back(pos);
            ast.id = id;
        }
    };*/
}