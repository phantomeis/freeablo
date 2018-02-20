
#pragma once

#include "../engine/inputobserverinterface.h"
#include "behaviour.h"

namespace FAWorld
{
    class PlayerBehaviour : public Behaviour, public Engine::MouseInputObserverInterface
    {
    public:
        static const std::string typeId;
        const std::string& getTypeId() override { return typeId; }

        PlayerBehaviour(FAWorld::Actor* actor);
        PlayerBehaviour() = default;

        virtual void save(FASaveGame::GameSaver&) override {}
        virtual void reAttach(Actor* actor) override;
        virtual void update() override;

        virtual void notify(Engine::MouseInputAction action, Misc::Point mousePosition, bool mouseDown, const Input::KeyboardModifiers& modifiers) override;

        virtual ~PlayerBehaviour() {}

        void blockInput();
        void unblockInput();

    private:
        FAWorld::Player* mPlayer = nullptr;
        int32_t mInputBlockedFramesLeft = 0;
        bool mUnblockInput = false;
        bool mTargetLock = false;
    };
}
