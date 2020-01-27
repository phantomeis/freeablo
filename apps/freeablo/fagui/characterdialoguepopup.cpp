#include "characterdialoguepopup.h"
#include "../faworld/actor.h"
#include "guimanager.h"

namespace FAGui
{
    CharacterDialoguePopup::CharacterDialoguePopup(GuiManager& guiManager, bool wide) : mGuiManager(guiManager), mWide(wide) {}

    CharacterDialoguePopup::UpdateResult CharacterDialoguePopup::update(struct nk_context* ctx)
    {
        auto renderer = FARender::Renderer::get();

        auto boxTex = renderer->loadImage(this->mWide ? "data/textbox.cel" : "data/textbox2.cel");
        int32_t screenW, screenH;
        renderer->getWindowDimensions(screenW, screenH);

        nk_flags flags = NK_WINDOW_NO_SCROLLBAR;

        auto dialogRectangle =
            nk_rect(screenW / 2.0f - (boxTex->getWidth() / 2.0f), screenH / 2.0f - (boxTex->getHeight() / 2.0f), boxTex->getWidth(), boxTex->getHeight());

        UpdateResult result = UpdateResult::DoNothing;

        mGuiManager.nk_fa_begin_image_window(
            ctx,
            "dialog",
            dialogRectangle,
            flags,
            boxTex->getNkImage(),
            [&]() {
                drawBackgroundCheckerboard(renderer, ctx, dialogRectangle);

                nk_layout_row_dynamic(ctx, 30, 1);

                DialogData data = getDialogData();

                for (const auto& line : data.introduction)
                    GuiManager::smallText(ctx, line.entry.c_str(), line.textColor);

                // fill the rest of the window
                struct nk_rect bounds = nk_widget_bounds(ctx);
                struct nk_rect panelSize = nk_window_get_bounds(ctx);
                float contentHeight = panelSize.h + panelSize.y - bounds.y;
                nk_layout_row_dynamic(ctx, contentHeight, 1);

                Misc::ScopedSetter<nk_style> styleSetter(ctx->style);

                // setup style for scrollbar
                {
                    constexpr int32_t scrollUpButtonIndex = 9;
                    constexpr int32_t scrollDownButtonIndex = 8;
                    constexpr int32_t scrollbarBackgroundIndex = 13;

                    // TODO: the "cursor" of the scrollbar is still the default nuklear look, which should change

                    auto sliderImg = renderer->loadImage("data/textslid.cel");

                    // TODO: this image should repeat, not stretch
                    ctx->style.scrollv.normal = nk_style_item_image(sliderImg->getNkImage(scrollbarBackgroundIndex));

                    ctx->style.scrollv.active = ctx->style.scrollv.normal;
                    ctx->style.scrollv.hover = ctx->style.scrollv.normal;
                    ctx->style.scrollv.cursor_active = ctx->style.scrollv.cursor_normal;
                    ctx->style.scrollv.cursor_hover = ctx->style.scrollv.cursor_normal;

                    ctx->style.scrollv.show_buttons = true;

                    ctx->style.scrollv.dec_button.normal = nk_style_item_image(sliderImg->getNkImage(scrollUpButtonIndex));
                    ctx->style.scrollv.dec_symbol = NK_SYMBOL_NONE;
                    ctx->style.scrollv.inc_button.normal = nk_style_item_image(sliderImg->getNkImage(scrollDownButtonIndex));
                    ctx->style.scrollv.inc_symbol = NK_SYMBOL_NONE;

                    ctx->style.scrollv.dec_button.active = ctx->style.scrollv.dec_button.normal;
                    ctx->style.scrollv.dec_button.hover = ctx->style.scrollv.dec_button.normal;

                    ctx->style.scrollv.inc_button.active = ctx->style.scrollv.inc_button.normal;
                    ctx->style.scrollv.inc_button.hover = ctx->style.scrollv.inc_button.normal;

                    ctx->style.window.group_padding = nk_vec2(10, 10);

                    ctx->style.window.scrollbar_size = nk_vec2(sliderImg->getWidth(), 0);
                }

                nk_group_scrolled_begin(ctx, &mScroll, "dialog_main", 0);
                {
                    if (mDialogMenu.update(ctx, data.dialogOptions, mScroll) == MouseAndClickMenu::Result::Activated)
                        result = data.dialogActions[mDialogMenu.getSelectedIndex()]();
                }
                nk_group_scrolled_end(ctx);
            },
            true);

        return result;
    }

    void CharacterDialoguePopup::drawBackgroundCheckerboard(FARender::Renderer* renderer, struct nk_context* ctx, struct nk_rect dialogRectangle)
    {
        auto blackTex = renderer->loadImage("resources/black.png");
        auto cbRect = nk_rect(dialogRectangle.x + 3, dialogRectangle.y + 3, dialogRectangle.w - 6, dialogRectangle.h - 6);

        ScopedApplyEffect effect(ctx, EffectType::checkerboarded);
        auto nkImage = nk_subimage_handle(blackTex->getNkImage().handle, blackTex->getWidth(), blackTex->getHeight(), cbRect);
        nk_draw_image(nk_window_get_canvas(ctx), cbRect, &nkImage, nk_rgb(0, 0, 0));
    }

    class TalkDialogPopup : public CharacterDialoguePopup
    {
    public:
        TalkDialogPopup(GuiManager& guiManager, const std::string& text) : CharacterDialoguePopup(guiManager, true), mText(text) {}

        virtual UpdateResult update(struct nk_context* ctx) override
        {
            auto renderer = FARender::Renderer::get();

            auto boxTex = renderer->loadImage("data/textbox.cel");
            int32_t screenW, screenH;
            renderer->getWindowDimensions(screenW, screenH);

            nk_flags flags = NK_WINDOW_NO_SCROLLBAR;

            auto dialogRectangle =
                nk_rect(screenW / 2.0f - (boxTex->getWidth() / 2.0f), screenH / 2.0f - (boxTex->getHeight() / 2.0f), boxTex->getWidth(), boxTex->getHeight());

            UpdateResult result = UpdateResult::DoNothing;

            mGuiManager.nk_fa_begin_image_window(
                ctx,
                "talkPopup",
                dialogRectangle,
                flags,
                boxTex->getNkImage(),
                [&]() {
                    drawBackgroundCheckerboard(renderer, ctx, dialogRectangle);

                    // fill the rest of the window
                    struct nk_rect bounds = nk_widget_bounds(ctx);
                    struct nk_rect panelSize = nk_window_get_bounds(ctx);
                    float contentHeight = panelSize.h + panelSize.y - bounds.y;
                    nk_layout_row_dynamic(ctx, contentHeight, 1);

                    auto wrapText = [](nk_context* ctx, const char* text, TextColor color) {
                        FARender::Renderer* renderer = FARender::Renderer::get();
                        nk_style_push_font(ctx, renderer->bigTGoldFont());
                        nk_style_push_color(ctx, &ctx->style.text.color, getNkColor(color));
                        nk_label_wrap(ctx, text);
                        nk_style_pop_color(ctx);
                        nk_style_pop_font(ctx);
                    };

                    auto& world = mGuiManager.mDialogManager.mWorld;
                    static auto startTime = world.getCurrentTick();
                    wrapText(ctx, mText.c_str(), TextColor::white);
                    auto currentTime = world.getCurrentTick();
                    if (currentTime - startTime >= world.getTicksInPeriod("0.1"))
                    {
                        ctx->active->scrollbar.y++;
                        startTime = currentTime;
                    }
                },
                true);

            return result;
        }

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;

            retval.introduction = {};

            return retval;
        }

    private:
        std::string mText;
    };

    class TalkDialog : public CharacterDialoguePopup
    {
    public:
        TalkDialog(GuiManager& guiManager, const FAWorld::Actor* actor) : CharacterDialoguePopup(guiManager, false), mActor(actor) {}

    protected:
        virtual DialogData getDialogData() override
        {
            DialogData retval;

            retval.introduction = {{mActor->getTalkData().at("talk"), TextColor::golden, false}};
            retval.addMenuOption({{"Gossip", TextColor::blue, true}}, [this]() {
                auto data = mActor->getGossipData();
                auto iterator = data.begin();
                std::advance(iterator, rand() % data.size());
                TalkDialogPopup* popup = new TalkDialogPopup(mGuiManager, iterator->second);
                openTalkPopup(popup);
                return CharacterDialoguePopup::UpdateResult::DoNothing;
            });

            return retval;
        }

        const FAWorld::Actor* mActor = nullptr;

    private:
        void openTalkPopup(TalkDialogPopup* popup) { mGuiManager.mDialogManager.pushDialog(popup); }
    };

    void CharacterDialoguePopup::openTalkDialog(const FAWorld::Actor* actor)
    {
        TalkDialog* dialog = new TalkDialog(mGuiManager, actor);
        mGuiManager.mDialogManager.pushDialog(dialog);
    }
}
