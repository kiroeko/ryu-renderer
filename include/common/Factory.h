#ifndef __FACTORY_H__
#define __FACTORY_H__

#include "common/Singleton.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <utility>

namespace OGLRenderer::Common
{
    // CRTP
    template <typename P, typename F>
    class Factory
    {
    template <typename T, typename... Args>
    static constexpr bool HasBeforeCreate = requires(T t, Args&&... args) {
        { t.BeforeCreate(std::forward<Args>(args)...) } -> std::same_as<bool>;
    };
    public:
        template <typename... Args>
        requires std::constructible_from<P, Args...> &&
                 std::derived_from<F, Factory<P, F>> &&
                 HasBeforeCreate<F, Args...>
        std::shared_ptr<P> Create(Args&&... args)
        {
            if (!static_cast<F*>(this)->BeforeCreate(std::forward<Args>(args)...))
                return nullptr;

            std::shared_ptr<P> p = std::make_shared<P>(std::forward<Args>(args)...);
            if (p)
            {
                products.emplace_back(p);
                AfterCreate(p);
            }

            return p;
        }

        void Remove(const std::shared_ptr<P>& p)
        {
            products.remove(p);
        }

        template <typename Pred>
        void RemoveAll(Pred pred)
        {
            products.remove_if(pred);
        }

        template <typename Pred>
        std::shared_ptr<P> FindAny(Pred pred)
        {
            auto it = std::find_if(products.begin(), products.end(), pred);
            return (it != products.end()) ? *it : nullptr;
        }

        template <typename Pred>
        std::list<std::shared_ptr<P>> FindAll(Pred pred)
        {
            std::vector<std::shared_ptr<P>> matched;
            std::copy_if(
                products.begin(),
                products.end(),
                std::back_inserter(matched),
                pred
            );
            return matched;
        }
        
        void Clear()
        {
            products.clear();
        }
        
        size_t Count()
        {
            return products.size();
        }
    protected:
        virtual void AfterCreate(const std::shared_ptr<P>& p) noexcept {}
    private:
        // Derived class need to impl this member function:
        // bool BeforeCreate(Args&&... args) { /* do judgement */ }

        std::list<std::shared_ptr<P>> products;
    };
}

#endif