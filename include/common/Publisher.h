#include <unordered_map>
#include <functional>
#include <any>
#include <typeindex>
#include <utility>

namespace RyuRenderer::Common
{
    class Publisher
    {
    public:
        template <typename MessageType>
        void RegisterHandler(std::function<void(const MessageType&)> handler)
        {
            auto key = std::type_index(typeid(MessageType));

            messages[key] =
                [handler = std::move(handler)](const std::any& event)
                {
                    handler(std::any_cast<const MessageType&>(event));
                };
        }

        template <typename MessageType>
        bool Dispatch(const MessageType& message)
        {
            auto key = std::type_index(typeid(MessageType));
            auto it = messages.find(key);

            if (it == messages.end())
                return false;

            it->second(message);
            return true;
        }

    private:
        std::unordered_map<std::type_index, std::function<void(const std::any&)>> messages;
    };
}