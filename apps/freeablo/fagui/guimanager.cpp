#include "guimanager.h"
#include "../engine/enginemain.h"
#include "../engine/localinputhandler.h"
#include "../farender/renderer.h"
#include "../fasavegame/gameloader.h"
#include "../faworld/actorstats.h"
#include "../faworld/equiptarget.h"
#include "../faworld/itemenums.h"
#include "../faworld/player.h"
#include "../faworld/world.h"
#include "boost/range/counting_range.hpp"
#include "dialogmanager.h"
#include "fa_nuklear.h"
#include "menu/multiplayerconnecting.h"
#include "menu/pausemenuscreen.h"
#include "menu/startingmenuscreen.h"
#include "menuhandler.h"
#include "nkhelpers.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/variant/variant.hpp>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <misc/misc.h>
#include <serial/textstream.h>
#include <string>

static nk_style_button dummyStyle = []() {
    static nk_style_button buttonStyle;
    buttonStyle.normal = buttonStyle.hover = buttonStyle.active = nk_style_item_hide();
    buttonStyle.border_color = {0, 0, 0, 0};
    buttonStyle.border = 0;
    buttonStyle.padding = {0, 0};
    return buttonStyle;
}();

namespace FAGui
{
    PanelPlacement panelPlacementByType(PanelType type)
    {
        switch (type)
        {
            case PanelType::none:
                return PanelPlacement::none;
            case PanelType::inventory:
                return PanelPlacement::right;
            case PanelType::spells:
                return PanelPlacement::right;
            case PanelType::character:
                return PanelPlacement::left;
            case PanelType::quests:
                return PanelPlacement::left;
        }
        return PanelPlacement::none;
    }

    const char* bgImgPath(PanelType type)
    {
        switch (type)
        {
            case PanelType::none:
                break;
            case PanelType::inventory:
                return "data/inv/inv.cel";
            case PanelType::spells:
                return "data/spellbk.cel";
            case PanelType::character:
                return "data/char.cel";
            case PanelType::quests:
                return "data/quest.cel";
        }
        return nullptr;
    }

    const char* panelName(PanelType type)
    {
        switch (type)
        {
            case PanelType::none:
                return "none";
            case PanelType::inventory:
                return "inventory";
            case PanelType::spells:
                return "spells";
            case PanelType::character:
                return "character";
            case PanelType::quests:
                return "quests";
        }
        return "";
    }

    GuiManager::GuiManager(Engine::EngineMain& engine) : mDialogManager(*this, *engine.mWorld.get()), mEngine(engine)
    {
        mMenuHandler.reset(new MenuHandler(engine));

        auto renderer = FARender::Renderer::get();
        mSmallPentagram.reset(new FARender::AnimationPlayer());
        mSmallPentagram->playAnimation(
            renderer->loadImage("data/pentspn2.cel"), FAWorld::World::getTicksInPeriod("0.06"), FARender::AnimationPlayer::AnimationType::Looped);

        startingScreen();
    }

    GuiManager::~GuiManager() {}

    void
    GuiManager::nk_fa_begin_window(nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, std::function<void(void)> action, bool isModal)
    {
        nk_style_push_vec2(ctx, &ctx->style.window.padding, nk_vec2(0, 0));
        if (mEngine.isPaused() || (isModalDlgShown() && !isModal))
            flags |= NK_WINDOW_NO_INPUT;
        if (nk_begin(ctx, title, bounds, flags))
        {
            action();
        }

        nk_end(ctx);

        nk_style_pop_vec2(ctx);
    }

    void GuiManager::nk_fa_begin_image_window(
        nk_context* ctx, const char* title, struct nk_rect bounds, nk_flags flags, struct nk_image background, std::function<void(void)> action, bool isModal)
    {
        nk_style_push_style_item(ctx, &ctx->style.window.fixed_background, nk_style_item_image(background));
        nk_fa_begin_window(ctx, title, bounds, flags, action, isModal);
        nk_style_pop_style_item(ctx);
    }

    void GuiManager::updateAnimations()
    {
        for (auto& anim : {mSmallPentagram.get()})
            anim->update();
    }

    void GuiManager::drawPanel(nk_context* ctx, PanelType panelType, std::function<void(void)> op)
    {
        PanelPlacement placement = panelPlacementByType(panelType);
        bool shown = *getPanelAtLocation(placement) == panelType;

        nk_window_show(ctx, panelName(panelType), shown ? NK_SHOWN : NK_HIDDEN);
        if (!shown)
            return;
        auto renderer = FARender::Renderer::get();
        auto invTex = renderer->loadImage(bgImgPath(panelType));
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        struct nk_rect dims = nk_rect(
            [&]()
            {
                switch (placement)
                {
                    case PanelPlacement::none:
                        break;
                    case PanelPlacement::left:
                        return 0;
                    case PanelPlacement::right:
                        return screenW - invTex->getWidth();
                }
                return 0;
            }(),
            (screenH - 125 - invTex->getHeight()) / 2,
            invTex->getWidth(),
            invTex->getHeight());
        nk_flags flags = NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND;

        nk_fa_begin_image_window(ctx, panelName(panelType), dims, flags, invTex->getNkImage(), op, false);
    }

    void GuiManager::triggerItem(const FAWorld::EquipTarget& target)
    {
        auto& item = mPlayer->mInventory.getItemAt(target);
        if (!item.isUsable())
            return;

        mGoldSplitTarget = nullptr;

        switch (item.getType())
        {
            case FAWorld::ItemType::gold:
            {
                mGoldSplitTarget = &item;
                mGoldSplitCnt = 0;
                break;
            }
            default:
                break;
        }
    }

    void GuiManager::item(nk_context* ctx, FAWorld::EquipTarget target, boost::variant<struct nk_rect, struct nk_vec2> placement, ItemHighlightInfo highlight)
    {
        auto& inv = mPlayer->mInventory;
        using namespace FAWorld;
        if (!inv.getCursorHeld().isEmpty())
            highlight = ItemHighlightInfo::notHighlighed;
        bool checkerboarded = false;

        auto& item = inv.getItemAt(target);
        if (item.isEmpty())
            return;
        if (!item.mIsReal)
        {
            if (item.getEquipLoc() == FAWorld::ItemEquipType::twoHanded && target.type == FAWorld::EquipTargetType::rightHand)
            {
                checkerboarded = true;
                highlight = ItemHighlightInfo::notHighlighed;
            }
            else
                return;
        }

        auto renderer = FARender::Renderer::get();

        auto frame = item.getGraphicValue();
        auto imgPath = "data/inv/objcurs.cel";
        auto sprite = renderer->loadImage(imgPath);
        auto img = sprite->getNkImage(frame);
        auto w = sprite->getWidth(frame);
        auto h = sprite->getHeight(frame);
        bool isHighlighted = (highlight == ItemHighlightInfo::highlited);

        boost::apply_visitor(
            Misc::overload(
                [&](const struct nk_rect& rect) { nk_layout_space_push(ctx, alignRect(nk_rect(0, 0, w, h), rect, halign_t::center, valign_t::center)); },
                [&](const struct nk_vec2& point) {
                    if (!item.mIsReal)
                        return;
                    nk_layout_space_push(ctx, nk_rect(point.x, point.y, w, h));
                    if (highlight == ItemHighlightInfo::highlightIfHover)
                    {
                        nk_button_label_styled(ctx, &dummyStyle, "");
                        if (isLastWidgetHovered(ctx))
                        {
                            isHighlighted = true;
                        }
                    }
                }),
            placement);
        auto effectType = isHighlighted ? EffectType::highlighted : EffectType::none;
        effectType = checkerboarded ? EffectType::checkerboarded : effectType;
        if (isHighlighted)
            mHoveredInventoryItemText = item.getFullDescription();
        ScopedApplyEffect effect(ctx, effectType);
        nk_image(ctx, img);
        if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_RIGHT) && mPlayer->mInventory.getCursorHeld().isEmpty())
            triggerItem(target);
    }

    void GuiManager::inventoryPanel(nk_context* ctx)
    {
        using namespace FAWorld;
        drawPanel(ctx, PanelType::inventory, [&]() {
            static std::vector<std::pair<EquipTarget, struct nk_rect>> slotRectangles = {
                {MakeEquipTarget<FAWorld::EquipTargetType::head>(), nk_rect(133, 3, 56, 56)},
                {MakeEquipTarget<FAWorld::EquipTargetType::amulet>(), nk_rect(205, 32, 28, 28)},
                {MakeEquipTarget<FAWorld::EquipTargetType::body>(), nk_rect(133, 75, 58, 87)},
                {MakeEquipTarget<FAWorld::EquipTargetType::leftHand>(), nk_rect(18, 75, 56, 84)},
                {MakeEquipTarget<FAWorld::EquipTargetType::rightHand>(), nk_rect(249, 75, 56, 84)},
                {MakeEquipTarget<FAWorld::EquipTargetType::leftRing>(), nk_rect(47, 178, 28, 28)},
                {MakeEquipTarget<FAWorld::EquipTargetType::rightRing>(), nk_rect(248, 178, 28, 28)}};

            FAWorld::CharacterInventory& inv = mPlayer->mInventory;
            nk_layout_space_begin(ctx, NK_STATIC, 0, INT_MAX);
            {
                for (auto& p : slotRectangles)
                {
                    nk_layout_space_push(ctx, p.second);
                    nk_button_label_styled(ctx, &dummyStyle, "");
                    if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT))
                    {
                        FAWorld::PlayerInput::InventorySlotClickedData input{p.first};
                        Engine::EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(input, mPlayer->getId()));
                    }
                    auto highlight = ItemHighlightInfo::notHighlighed;
                    if (isLastWidgetHovered(ctx))
                        highlight = ItemHighlightInfo::highlited;

                    item(ctx, p.first, p.second, highlight);
                }
            }
            constexpr float cellSize = 29;
            auto invTopLeft = nk_vec2(17, 222);

            const FAWorld::BasicInventory& mainInventory = inv.getInv(FAWorld::EquipTargetType::inventory);

            float invWidth = mainInventory.width() * cellSize;
            float invHeight = mainInventory.height() * cellSize;
            nk_layout_space_push(ctx, nk_recta(invTopLeft, {invWidth, invHeight}));
            nk_button_label_styled(ctx, &dummyStyle, "");
            if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT) && !mGoldSplitTarget)
            {
                // Adjust for cursor offset when items are held.
                // When items are held, their sprites are centered around the cursor (rather then top left).
                auto cursorOffset = nk_vec2(0, 0);
                auto item = mPlayer->mInventory.getCursorHeld();
                if (!item.isEmpty())
                {
                    auto invSize = item.getInvSize();
                    cursorOffset = {(1 - invSize[0]) * cellSize / 2, (1 - invSize[1]) * cellSize / 2};
                }
                Misc::Point clickedPoint{int32_t(std::floor((ctx->input.mouse.pos.x - invTopLeft.x - ctx->current->bounds.x + cursorOffset.x) / cellSize)),
                                         int32_t(std::floor((ctx->input.mouse.pos.y - invTopLeft.y - ctx->current->bounds.y + cursorOffset.y) / cellSize))};

                FAWorld::PlayerInput::InventorySlotClickedData input{
                    FAWorld::MakeEquipTarget<FAWorld::EquipTargetType::inventory>(clickedPoint.x, clickedPoint.y)};
                Engine::EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(input, mPlayer->getId()));
            }

            for (auto row : boost::counting_range(0, mainInventory.height()))
                for (auto col : boost::counting_range(0, mainInventory.width()))
                {
                    auto cell_top_left = nk_vec2(invTopLeft.x + col * cellSize, invTopLeft.y + row * cellSize);
                    item(ctx, MakeEquipTarget<FAWorld::EquipTargetType::inventory>(col, row), cell_top_left, ItemHighlightInfo::highlightIfHover);
                }
            if (mGoldSplitTarget)
            {
                int32_t screenW, screenH;
                auto renderer = FARender::Renderer::get();
                renderer->getWindowDimensions(screenW, screenH);
                auto img = renderer->loadImage("ctrlpan/golddrop.cel");
                double leftTopX = 31.0, leftTopY = 42.0;
                nk_layout_space_push(ctx, nk_rect(leftTopX, leftTopY, img->getWidth(), img->getHeight()));
                nk_image(ctx, img->getNkImage());
                auto spacing = 28.0;
                auto doTextLine = [&](const char* text, double y) {
                    nk_layout_space_push(ctx, nk_rect(leftTopX + spacing, y, img->getWidth() - 2 * spacing, renderer->smallFont()->height));
                    smallText(ctx, text, TextColor::golden, NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_TOP);
                };
                doTextLine((boost::format("You have %1% gold") % mGoldSplitTarget->mCount).str().c_str(), 76.0);
                doTextLine((boost::format("%1%.  How many do") % (mGoldSplitTarget->mCount > 1 ? "pieces" : "piece")).str().c_str(), 92.0);
                doTextLine("you want to remove?", 110.0);
                {
                    auto offset = 6;
                    nk_layout_space_push(ctx, nk_rect(leftTopX + spacing + offset, 129.0, img->getWidth() - 2 * spacing, renderer->smallFont()->height));
                    auto text = mGoldSplitCnt ? std::to_string(mGoldSplitCnt) : "";
                    smallText(ctx, text.c_str(), TextColor::white, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_TOP);
                    auto width = smallTextWidth(text.c_str());
                    auto spr = mSmallPentagram->getCurrentFrame().first;
                    nk_layout_space_push(ctx, nk_rect(leftTopX + spacing + offset + width, 129.0, spr->getWidth(), spr->getHeight()));
                    nk_image(ctx, mSmallPentagram->getCurrentNkImage());
                }
            }
            nk_layout_space_end(ctx);
        });
    }

    void GuiManager::fillTextField(nk_context* ctx, float x, float y, float width, const char* text, TextColor color)
    {
        nk_layout_space_push(ctx, nk_rect(x, y, width, FARender::Renderer::get()->smallFont()->height));
        smallText(ctx, text, color);
    }

    void GuiManager::characterPanel(nk_context* ctx)
    {
        drawPanel(ctx, PanelType::character, [&]() {
            nk_layout_space_begin(ctx, NK_STATIC, 0, INT_MAX);

            const FAWorld::ActorStats& playerStats = mPlayer->getStats();
            const FAWorld::LiveActorStats& liveStats = playerStats.getCalculatedStats();

            fillTextField(ctx, 168, 21, 131, toString(mPlayer->getClass()));

            fillTextField(ctx, 95, 144, 31, std::to_string(playerStats.baseStats.strength).c_str());
            fillTextField(ctx, 142, 144, 31, std::to_string(liveStats.baseStats.strength).c_str());

            fillTextField(ctx, 95, 172, 31, std::to_string(playerStats.baseStats.magic).c_str());
            fillTextField(ctx, 142, 172, 31, std::to_string(liveStats.baseStats.magic).c_str());

            fillTextField(ctx, 95, 200, 31, std::to_string(playerStats.baseStats.dexterity).c_str());
            fillTextField(ctx, 142, 200, 31, std::to_string(liveStats.baseStats.dexterity).c_str());

            fillTextField(ctx, 95, 228, 31, std::to_string(playerStats.baseStats.vitality).c_str());
            fillTextField(ctx, 142, 228, 31, std::to_string(playerStats.baseStats.vitality).c_str());

            fillTextField(ctx, 73, 59, 31, std::to_string(playerStats.mLevel).c_str());
            fillTextField(ctx, 216, 59, 84, std::to_string(playerStats.mExperience).c_str());
            fillTextField(ctx, 216, 87, 84, std::to_string(playerStats.nextLevelExperience()).c_str());

            fillTextField(ctx, 216, 135, 84, std::to_string(mPlayer->mInventory.getTotalGold()).c_str());
            fillTextField(ctx, 95, 293, 31, std::to_string(playerStats.mHp.max).c_str());
            fillTextField(ctx,
                          143,
                          293,
                          31,
                          std::to_string(playerStats.mHp.current).c_str(),
                          playerStats.mHp.current < playerStats.mHp.max ? TextColor::red : TextColor::white);
            fillTextField(ctx, 95, 321, 31, std::to_string(playerStats.mMana.max).c_str());
            fillTextField(ctx,
                          143,
                          321,
                          31,
                          std::to_string(playerStats.mMana.current).c_str(),
                          playerStats.mMana.current < playerStats.mMana.max ? TextColor::red : TextColor::white);

            fillTextField(ctx, 257, 172, 43, std::to_string(liveStats.armorClass).c_str());
            fillTextField(ctx, 257, 200, 43, std::to_string(liveStats.toHitMelee.base).c_str());

            nk_layout_space_end(ctx);
        });
    }

    void GuiManager::questsPanel(nk_context* ctx)
    {
        drawPanel(ctx, PanelType::quests, [&]() {});
    }

    void GuiManager::spellsPanel(nk_context* ctx)
    {
        drawPanel(ctx, PanelType::spells, [&]() {});
    }

    void GuiManager::belt(nk_context* ctx)
    {
        auto beltTopLeft = nk_vec2(205, 21);
        auto beltWidth = 232.0f, beltHeight = 29.0f, cellSize = 29.0f;
        nk_layout_space_push(ctx, nk_recta(beltTopLeft, {beltWidth, beltHeight}));
        auto& inv = mPlayer->mInventory;
        if (nk_widget_is_mouse_click_down_inactive(ctx, NK_BUTTON_LEFT) && !mGoldSplitTarget)
        {
            FAWorld::PlayerInput::InventorySlotClickedData input{FAWorld::MakeEquipTarget<FAWorld::EquipTargetType::belt>(
                int32_t(std::floor(ctx->input.mouse.pos.x - beltTopLeft.x - ctx->current->bounds.x) / cellSize))};
            Engine::EngineMain::get()->getLocalInputHandler()->addInput(FAWorld::PlayerInput(input, mPlayer->getId()));
        }

        using namespace FAWorld;
        for (auto num : boost::counting_range(0, int32_t(inv.getInv(FAWorld::EquipTargetType::belt).width())))
        {
            auto cell_top_left = nk_vec2(beltTopLeft.x + num * cellSize, beltTopLeft.y);
            item(ctx, MakeEquipTarget<FAWorld::EquipTargetType::belt>(num), cell_top_left, ItemHighlightInfo::highlightIfHover);
        }
        nk_layout_space_end(ctx);
    }

    void GuiManager::bottomMenu(nk_context* ctx, const FAWorld::HoverStatus& hoverStatus)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();

        // The bottom menu is made of two sprites: panel8.cel, which is the background,
        // and panel8bu.cel, which contains overlays for each button. It's pretty primitive,
        // the buttons are baked into the background image.
        FARender::FASpriteGroup* bottomMenuTex = renderer->loadImage("ctrlpan/panel8.cel");
        FARender::FASpriteGroup* bottomMenuButtonsTex = renderer->loadImage("ctrlpan/panel8bu.cel");
        FARender::FASpriteGroup* healthAndManaEmptyBulbs = renderer->loadImage("ctrlpan/p8bulbs.cel");

        int32_t bulbWidth = healthAndManaEmptyBulbs->getWidth();
        int32_t bulbHeight = healthAndManaEmptyBulbs->getHeight();

        int32_t bottomMenuWidth = bottomMenuTex->getWidth();
        int32_t bottomMenuHeight = bottomMenuTex->getHeight();

        // Magic numbers defining the positions of the buttons on the background
        int32_t buttonLeftIndent = 10;
        int32_t buttonRightIndent = 560;

        int32_t buttonWidth = 71;
        int32_t buttonHeight = 19;

        int32_t buttonRow1TopIndent = 25;
        int32_t buttonRow2TopIndent = 52;
        int32_t buttonRow3TopIndent = 92;
        int32_t buttonRow4TopIndent = 118;

        // indices into panel8bu.cel
        int32_t charButtonFrame = 0;
        int32_t questButtonFrame = 1;
        int32_t mapButtonFrame = 2;
        int32_t menuButtonFrame = 3;
        int32_t invButtonFrame = 4;
        int32_t spellsButtonFrame = 5;

        int32_t healthBulbLeftOffset = 96;
        int32_t manaBulbLeftOffset = 464;

        // Centre the bottom menu on the bottom of the screen
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);
        struct nk_rect dims = nk_rect((screenW / 2) - (bottomMenuWidth / 2), screenH - bottomMenuHeight, bottomMenuWidth, bottomMenuHeight);

        Misc::ScopedSetter<float> setter(ctx->style.button.border, 0);
        Misc::ScopedSetter<struct nk_style_item> setter2(ctx->style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));

        nk_fa_begin_window(ctx,
                           "bottom_menu",
                           dims,
                           NK_WINDOW_NO_SCROLLBAR,
                           [&]() {
                               nk_layout_space_begin(ctx, NK_STATIC, buttonHeight, INT_MAX);

                               auto drawBgSection = [&](struct nk_rect rect) {
                                   struct nk_image section = nk_subimage_handle(bottomMenuTex->getNkImage(0).handle, bottomMenuWidth, bottomMenuHeight, rect);
                                   nk_layout_space_push(ctx, rect);
                                   nk_image(ctx, section);
                               };

                               // The bottom panel background image has full health and mana orbs baked into the image.
                               // The empty orbs are contained in separate images, and include the background, clearly intended
                               // to be drawn on top of the backgorund image. That works fine if you're a dumb cpu loop overwriting pixels,
                               // but we're doing a normal draw with an alpha channel because it's not 1996 anymore. So, what we want is
                               // the background image with empty health bulbs, on top of which we can draw the filled ones.
                               // So, we draw the whole panel background excluding the bulbs, then separately draw the empty bulbs.

                               // Leftmost section
                               drawBgSection(nk_rect(0, 0, healthBulbLeftOffset, bottomMenuHeight));
                               // Below left bulb section
                               drawBgSection(nk_rect(healthBulbLeftOffset, bulbHeight, bulbWidth, bottomMenuHeight - bulbHeight));
                               // Middle section
                               drawBgSection(
                                   nk_rect(healthBulbLeftOffset + bulbWidth, 0, manaBulbLeftOffset - (healthBulbLeftOffset + bulbWidth), bottomMenuHeight));
                               // Below right bulb section
                               drawBgSection(nk_rect(manaBulbLeftOffset, bulbHeight, bulbWidth, bottomMenuHeight - bulbHeight));
                               // Rightmost section
                               drawBgSection(nk_rect(manaBulbLeftOffset + bulbWidth, 0, bottomMenuWidth - manaBulbLeftOffset, bottomMenuHeight));

                               // Health bulb
                               nk_layout_space_push(ctx, nk_rect(healthBulbLeftOffset, 0, bulbWidth, bulbHeight));
                               nk_image(ctx, healthAndManaEmptyBulbs->getNkImage(0));

                               // Mana bulb
                               nk_layout_space_push(ctx, nk_rect(manaBulbLeftOffset, 0, bulbWidth, bulbHeight));
                               nk_image(ctx, healthAndManaEmptyBulbs->getNkImage(1));

                               nk_style_button buttonStyle = ctx->style.button;
                               // The "unpressed" version of the button is baked into the background image, so just draw nothing
                               buttonStyle.normal = nk_style_item_color(nk_rgba(0, 0, 0, 0));
                               buttonStyle.hover = nk_style_item_color(nk_rgba(0, 0, 0, 0));

                               auto bottomMenuButton = [&](int32_t topIndent, int32_t leftIndent, int32_t frame) {
                                   nk_layout_space_push(ctx, nk_rect(leftIndent, topIndent, buttonWidth, buttonHeight));
                                   buttonStyle.active = nk_style_item_image(bottomMenuButtonsTex->getNkImage(frame));
                                   return nk_button_label_styled(ctx, &buttonStyle, "");
                               };

                               // CHAR button
                               if (bottomMenuButton(buttonRow1TopIndent, buttonLeftIndent, charButtonFrame))
                                   togglePanel(PanelType::character);

                               // QUEST button
                               if (bottomMenuButton(buttonRow2TopIndent, buttonLeftIndent, questButtonFrame))
                                   togglePanel(PanelType::quests);

                               // MAP button
                               if (bottomMenuButton(buttonRow3TopIndent, buttonLeftIndent, mapButtonFrame))
                                   std::cout << "pressed MAP" << std::endl;

                               // MENU button
                               if (bottomMenuButton(buttonRow4TopIndent, buttonLeftIndent, menuButtonFrame))
                                   std::cout << "pressed MENU" << std::endl;

                               // INV button
                               if (bottomMenuButton(buttonRow1TopIndent, buttonRightIndent, invButtonFrame))
                                   togglePanel(PanelType::inventory);

                               // SPELLS button
                               if (bottomMenuButton(buttonRow2TopIndent, buttonRightIndent, spellsButtonFrame))
                                   togglePanel(PanelType::spells);

                               auto drawBulb = [&](float current, float max, int32_t leftOffset) {
                                   float percent = current / max;
                                   float useBulbHeight = bulbHeight * percent;

                                   struct nk_rect r = nk_rect(leftOffset, (bulbHeight - useBulbHeight), bulbWidth, useBulbHeight);
                                   nk_layout_space_push(ctx, r);

                                   struct nk_image bulbImage = nk_subimage_handle(bottomMenuTex->getNkImage(0).handle, bottomMenuWidth, bottomMenuHeight, r);
                                   nk_image(ctx, bulbImage);
                               };

                               const FAWorld::ActorStats& stats = Engine::EngineMain::get()->mWorld->getCurrentPlayer()->getStats();
                               // draw current hp into health bulb
                               drawBulb(stats.mHp.current, stats.mHp.max, healthBulbLeftOffset);
                               // and current mana
                               drawBulb(stats.mMana.current, stats.mMana.current, manaBulbLeftOffset);

                               belt(ctx);
                               descriptionPanel(ctx, hoverStatus.getDescription(*Engine::EngineMain::get()->mWorld->getCurrentLevel()));

                               nk_layout_space_end(ctx);
                           },
                           false);
    }

    void GuiManager::startingScreen() { mMenuHandler->setActiveScreen<StartingMenuScreen>(); }

    void GuiManager::connectingScreen() { mMenuHandler->setActiveScreen<MultiplayerConnecting>(); }

    void GuiManager::smallText(nk_context* ctx, const char* text, TextColor color, nk_flags alignment)
    {
        FARender::Renderer* renderer = FARender::Renderer::get();
        nk_style_push_font(ctx, renderer->smallFont());
        nk_style_push_color(ctx, &ctx->style.text.color, getNkColor(color));
        nk_label(ctx, text, alignment);
        nk_style_pop_color(ctx);
        nk_style_pop_font(ctx);
    }

    void GuiManager::smallText(nk_context* ctx, const char* text, TextColor color)
    {
        smallText(ctx, text, color, NK_TEXT_ALIGN_CENTERED | NK_TEXT_ALIGN_MIDDLE);
    }

    int GuiManager::smallTextWidth(const char* text)
    {
        auto renderer = FARender::Renderer::get();
        auto fnt = renderer->smallFont();
        return fnt->width(fnt->userdata, 0.0f, text, strlen(text));
    }

    void GuiManager::descriptionPanel(nk_context* ctx, const std::string& description)
    {
        const std::string* textToUse = &description;
        if (!mHoveredInventoryItemText.empty())
            textToUse = &mHoveredInventoryItemText;

        auto boxRect = nk_rect(185, 66, 275, 55);
        std::vector<std::string> vec;
        boost::split(vec, *textToUse, boost::is_any_of("\n"), boost::token_compress_on);
        auto h_part = boxRect.h / vec.size();
        for (int i = 0; i < static_cast<int>(vec.size()); ++i)
        {
            // TODO: copy precise location of several line positioning from exe
            auto rect = boxRect;
            rect.y = boxRect.y + h_part * i;
            rect.h = h_part;
            nk_layout_space_push(ctx, rect);
            smallText(ctx, vec[i].c_str(), TextColor::white);
        }
    }

    void GuiManager::update(bool inGame, bool paused, nk_context* ctx, const FAWorld::HoverStatus& hoverStatus)
    {
        nk_style_push_font(ctx, FARender::Renderer::get()->smallFont());

        // HACK FUCKING HACK
        FAWorld::World* world = Engine::EngineMain::get()->mWorld.get();
        mPlayer = world == nullptr ? nullptr : world->getCurrentPlayer();

        mHoveredInventoryItemText.clear();

        if (inGame)
        {
            if (!mPlayer)
                return;

            if (paused)
            {
                if (!mMenuHandler->isActive())
                    mMenuHandler->setActiveScreen<PauseMenuScreen>();

                mMenuHandler->update(ctx);
            }
            else if (mMenuHandler->isActive())
            {
                mMenuHandler->disable();
            }

            updateAnimations();
            inventoryPanel(ctx);
            spellsPanel(ctx);
            questsPanel(ctx);
            characterPanel(ctx);
            bottomMenu(ctx, hoverStatus);

            mDialogManager.update(ctx);
        }
        else
        {
            mMenuHandler->update(ctx);
        }

        if (isModalDlgShown() || mMenuHandler->isActive())
            Engine::EngineMain::get()->getLocalInputHandler()->blockInput();
        else
            Engine::EngineMain::get()->getLocalInputHandler()->unblockInput();

        nk_style_pop_font(ctx);
    }

    PanelType* GuiManager::getPanelAtLocation(PanelPlacement placement)
    {
        switch (placement)
        {
            case PanelPlacement::none:
                break;
            case PanelPlacement::left:
                return &mCurLeftPanel;
            case PanelPlacement::right:
                return &mCurRightPanel;
        }
        return nullptr;
    }

    const PanelType* GuiManager::getPanelAtLocation(PanelPlacement placement) const { return const_cast<self*>(this)->getPanelAtLocation(placement); }

    bool GuiManager::hotkeysEnabled() const
    {
        if (!mEngine.mInGame)
            return false;

        // Can't use hotkeys when dialogs are open.
        // TODO: mGoldSplitTarget might be better as a standard dialog if possible?
        if (mGoldSplitTarget || mDialogManager.hasDialog())
            return false;

        return true;
    }

    void GuiManager::notify(Engine::KeyboardInputAction action)
    {
        switch (action)
        {
            case Engine::KeyboardInputAction::toggleQuests:
                if (!hotkeysEnabled())
                    return;
                togglePanel(PanelType::quests);
                break;
            case Engine::KeyboardInputAction::toggleInventory:
                if (!hotkeysEnabled())
                    return;
                togglePanel(PanelType::inventory);
                break;
            case Engine::KeyboardInputAction::toggleCharacterInfo:
                if (!hotkeysEnabled())
                    return;
                togglePanel(PanelType::character);
                break;
            case Engine::KeyboardInputAction::toggleSpellbook:
                if (!hotkeysEnabled())
                    return;
                togglePanel(PanelType::spells);
                break;
            case Engine::KeyboardInputAction::reject:
                if (mGoldSplitTarget)
                    mGoldSplitTarget = nullptr;
                break;
            case Engine::KeyboardInputAction::accept:
                if (mGoldSplitTarget)
                {
                    if (mGoldSplitCnt > 0)
                    {
                        FAWorld::PlayerInput input(
                            FAWorld::PlayerInput::SplitGoldStackIntoCursorData{mGoldSplitTarget->mInvX, mGoldSplitTarget->mInvY, mGoldSplitCnt},
                            mPlayer->getId());
                        Engine::EngineMain::get()->getLocalInputHandler()->addInput(input);
                    }

                    mGoldSplitTarget = nullptr;
                }
                break;

            default:
                break;
        }

        mMenuHandler->notify(action);
    }

    void GuiManager::keyPress(const Input::Hotkey& key)
    {
        if (mGoldSplitTarget && !key.has_modifiers())
        {
            if (key.key >= '0' && key.key <= '9')
            {
                int digit = key.key - '0';
                auto newCnt = mGoldSplitCnt * 10 + digit;
                if (newCnt <= mGoldSplitTarget->mCount)
                    mGoldSplitCnt = newCnt;
            }
            else if (key.key == '\b')
            {
                mGoldSplitCnt /= 10;
            }
        }
    }

    void GuiManager::setPlayer(FAWorld::Player* player) { mPlayer = player; }

    bool GuiManager::isLastWidgetHovered(nk_context* ctx) const { return nk_inactive_widget_is_hovered(ctx) && !mEngine.isPaused(); }

    bool GuiManager::isInventoryShown() const { return *getPanelAtLocation(panelPlacementByType(PanelType::inventory)) == PanelType::inventory; }

    bool GuiManager::isPauseBlocked() const
    {
        if (mGoldSplitTarget)
            return true;
        return false;
    }

    bool GuiManager::isModalDlgShown() const { return mDialogManager.hasDialog(); }

    void GuiManager::popModalDlg() { mDialogManager.popDialog(); }

    bool GuiManager::anyPanelIsOpen() const { return (mCurLeftPanel != FAGui::PanelType::none || mCurRightPanel != FAGui::PanelType::none); }

    void GuiManager::closeAllPanels()
    {
        mCurLeftPanel = FAGui::PanelType::none;
        mCurRightPanel = FAGui::PanelType::none;
        mGoldSplitTarget = nullptr;
    }

    void GuiManager::togglePanel(PanelType type)
    {
        auto& curPanel = *getPanelAtLocation(panelPlacementByType(type));
        if (curPanel == type)
            curPanel = PanelType::none;
        else
            curPanel = type;
        if (mCurRightPanel != PanelType::inventory)
            mGoldSplitTarget = nullptr;
    }

    std::string cursorPath = "data/inv/objcurs.cel";
    uint32_t cursorFrame = 0;
}
