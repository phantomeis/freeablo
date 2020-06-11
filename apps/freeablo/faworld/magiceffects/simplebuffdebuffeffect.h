#pragma once
#include "magiceffect.h"

namespace FAWorld
{
    class SimpleBuffDebuffEffectBase;

    class SimpleBuffDebuffEffect : public MagicEffect
    {
        using super = MagicEffect;

    public:
        explicit SimpleBuffDebuffEffect(const SimpleBuffDebuffEffectBase* base);
        void init() override;

        void apply(MagicStatModifiers& modifiers) const override;
        std::string getFullDescription() const override;

        const SimpleBuffDebuffEffectBase* getBase() const;

    private:
        int32_t mValue = 0;
    };
}
