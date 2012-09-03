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
    struct set_annotation_id
    {
        typedef void result_type;

        size_t id;
        set_annotation_id(size_t id) : id(id) {}

        void operator()(ast::function_call& x) const
        {
            x.idf.id = id;
        }

        template <typename T>
        void dispatch(T& x, boost::mpl::true_) const
        {
            x.id = id;
        }

        template <typename T>
        void dispatch(T& x, boost::mpl::false_) const
        {
            // no-op
        }

        template <typename T>
        void operator()(T& x) const
        {
            typename boost::is_base_of<ast::tagged, T> is_tagged;
            dispatch(x, is_tagged);
        }
    };

    struct get_annotation_id
    {
        typedef size_t result_type;

        size_t operator()(ast::function_call& x) const
        {
            return x.idf.id;
        }

        template <typename T>
        size_t dispatch(T& x, boost::mpl::true_) const
        {
            return x.id;
        }

        template <typename T>
        size_t dispatch(T& x, boost::mpl::false_) const
        {
            return -1;
        }

        template <typename T>
        size_t operator()(T& x) const
        {
            typename boost::is_base_of<ast::tagged, T> is_tagged;
            return dispatch(x, is_tagged);
        }
    };

    template <typename Iterator>
    struct annotation
    {
        template <typename, typename>
        struct result { typedef void type; };

        std::vector<Iterator>& iters;
        annotation(std::vector<Iterator>& iters)
          : iters(iters) {}

        void operator()(ast::operand& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            boost::apply_visitor(set_annotation_id(id), ast);
            ast.id = id;
        }

        /*void operator()(ast::primary_expr& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            boost::apply_visitor(set_annotation_id(id), ast);
            ast.id = id;
        }*/

        void operator()(ast::variable_declaration& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            ast.lhs.id = id;
        }

        void operator()(ast::assignment& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            ast.lhs.id = id;
        }

        /*void operator()(ast::return_statement& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            ast.id = id;
        }*/

        void operator()(ast::identifier& ast, Iterator pos) const
        {
            size_t id = iters.size();
            iters.push_back(pos);
            ast.id = id;
        }
    };
}