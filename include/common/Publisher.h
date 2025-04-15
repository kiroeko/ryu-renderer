#include <unordered_map>
#include <functional>
#include <any>
#include <typeindex>
#include <vector>
#include <memory>
#include <stdexcept>

namespace RyuRenderer::Common
{
    class Publisher
    {
    public:
        template <typename EventType>
        size_t RegisterHandler(std::function<void(const EventType&)> handler)
        {
            auto key = std::type_index(typeid(EventType));
            size_t id = nextHandlerID++;
            auto wrapper =
                [handler = std::move(handler)](const std::any& event)
                {
                    handler(std::any_cast<const EventType&>(event));
                };

            handlers[key].emplace_back(id, std::move(wrapper));

            return id;
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

        template <typename EventType>
        void Dispatch(const EventType& event)
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
        std::unordered_map<
            std::type_index,
            std::vector<std::pair<size_t, std::function<void(const std::any&)>>>
        > handlers;

        size_t nextHandlerID = 0;
    };
}