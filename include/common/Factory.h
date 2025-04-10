#ifndef __FACTORY_H__
#define __FACTORY_H__

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <shared_mutex>
#include <utility>

#include "common/Singleton.h"

namespace RyuRenderer::Common
{
    // CRTP
    template <typename P, typename F>
    class Factory
    {
    template <typename T, typename... Args>
    static constexpr bool hasBeforeCreate = requires(T t, Args&&... args) {
        { t.BeforeCreate(std::forward<Args>(args)...) } -> std::same_as<bool>;
    };
    public:
        template <typename... Args>
        requires std::constructible_from<P, Args...> &&
                 std::derived_from<F, Factory<P, F>> &&
                 hasBeforeCreate<F, Args...>
        std::shared_ptr<P> Create(Args&&... args)
        {
            if (!static_cast<F*>(this)->BeforeCreate(std::forward<Args>(args)...))
                return nullptr;

            std::unique_lock<std::shared_mutex> lock(mutex);
            std::shared_ptr<P> p = std::make_shared<P>(std::forward<Args>(args)...);
            if (p)
            {
                products.emplace_back(p);
                lock.unlock();
                AfterCreate(p);
            }

            return p;
        }

        template <typename Pred>
        bool Remove(Pred pred)
        {
            auto it = std::find_if(products.begin(), products.end(), pred);
            if (it != products.end())
            {
                std::unique_lock<std::shared_mutex> lock(mutex);
                products.erase(it);
                return true;
            }

            return false;
        }

        template <typename Pred>
        size_t RemoveAll(Pred pred)
        {
            std::unique_lock<std::shared_mutex> lock(mutex);

            return products.remove_if(pred);
        }

        size_t RemoveAll(const std::shared_ptr<P>& p)
        {
            std::unique_lock<std::shared_mutex> lock(mutex);

            return products.remove(p);
        }

        template <typename Pred>
        std::shared_ptr<P> Find(Pred pred) const
        {
            std::shared_lock<std::shared_mutex> lock(mutex);

            auto it = std::find_if(products.begin(), products.end(), pred);
            return (it != products.end()) ? *it : nullptr;
        }

        template <typename Pred>
        std::list<std::shared_ptr<P>> FindAll(Pred pred) const
        {
            std::shared_lock<std::shared_mutex> lock(mutex);

            std::list<std::shared_ptr<P>> matched;
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
            std::unique_lock<std::shared_mutex> lock(mutex);

            products.clear();
        }
        
        size_t Count() const
        {
            return products.size();
        }
    protected:
        virtual void AfterCreate(const std::shared_ptr<P>& p) noexcept {}

        std::list<std::shared_ptr<P>> products;
    private:
        // Derived class need to impl this member function:
        // bool BeforeCreate(...) { /* do judgement */ }

        mutable std::shared_mutex mutex;
    };
}

#endif