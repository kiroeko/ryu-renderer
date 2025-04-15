#include <unordered_map>
#include <functional>
#include <any>
#include <typeindex>
#include <list>
#include <memory>

namespace RyuRenderer::Common
{
    class Publisher
    {
    public:
        template <typename EventType>
        size_t RegisterHandler(std::function<void(const EventType&)> handler)
        {
            return registerHandlerImpl<EventType>(std::move(handler));
        }

        template <typename T, typename EventType>
        size_t RegisterHandler(T* instance, void (T::* method)(const EventType&))
        {
            return registerHandlerImpl<EventType>(
                    [instance, method](const EventType& event)
                    {
                        (instance->*method)(event);
                    }
                );
        }

        template <typename T, typename EventType>
        size_t RegisterHandler(const T* instance, void (T::* method)(const EventType&) const)
        {
            return registerHandlerImpl<EventType>(
                    [instance, method](const EventType& event)
                    {
                        (instance->*method)(event);
                    }
                );
        }

        template <typename EventType>
        bool UnregisterHandler(size_t id)
        {
            auto key = std::type_index(typeid(EventType));
            auto it = handlers.find(key);

            if (it != handlers.end())
            {
                auto& handlers = it->second;
                for (auto handlerIt = handlers.begin(); handlerIt != handlers.end(); ++handlerIt)
                {
                    if (handlerIt->first == id)
                    {
                        handlers.erase(handlerIt);
                        return true;
                    }
                }
            }
            return false;
        }

        // 分发事件
        template <typename EventType>
        void Dispatch(const EventType& event) const
        {
            auto key = std::type_index(typeid(EventType));
            auto it = handlers.find(key);

            if (it != handlers.end())
            {
                for (const auto& [id, handler] : it->second)
                {
                    handler(event);
                }
            }
        }

        void Clear()
        {
            handlers.clear();
            nextHandlerID = 0;
        }
    private:
        template <typename EventType>
        size_t registerHandlerImpl(std::function<void(const EventType&)> handler)
        {
            auto key = std::type_index(typeid(EventType));
            auto wrapper =
                [handler = std::move(handler)](const std::any& event)
                {
                    handler(std::any_cast<const EventType&>(event));
                };

            size_t id = nextHandlerID++;
            handlers[key].emplace_back(id, std::move(wrapper));
            return id;
        }

        std::unordered_map<
            std::type_index,
            std::list<std::pair<size_t, std::function<void(const std::any&)>>>
        > handlers;

        size_t nextHandlerID = 0;
    };
}