/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/ui/brave_ads/ad_notification_view.h"

#include "base/path_service.h"
#include "brave/app/vector_icons/vector_icons.h"
#include "brave/grit/brave_theme_resources.h"
#include "brave/grit/brave_unscaled_resources.h"
#include "brave/ui/brave_ads/notification_widget.h"
#include "brave/ui/brave_ads/notification_background_painter.h"
#include "brave/ui/brave_ads/notification_buttons_view.h"
#include "brave/ui/brave_ads/notification_title_view.h"
#include "brave/ui/brave_ads/padded_image_button.h"
#include "brave/ui/brave_ads/public/cpp/constants.h"
#include "brave/ui/brave_ads/public/cpp/notification.h"
#include "build/build_config.h"
#include "components/url_formatter/elide_url.h"
#include "ui/base/class_property.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/events/base_event_utils.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/image/image_skia_operations.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/text_constants.h"
#include "ui/gfx/text_elider.h"
#include "ui/strings/grit/ui_strings.h"
#include "ui/views/background.h"
#include "ui/views/border.h"
#include "ui/views/controls/highlight_path_generator.h"
#include "ui/views/controls/image_view.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/flex_layout.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"

namespace brave_ads {

namespace {

constexpr gfx::Insets kInsideBorderInsets(0, 12, 10, 6);

constexpr gfx::Insets kBorderInsets(0, 0, 0, 0);

const int kBodyTextFontSize = 13;

const int kLineHeight = 17;

constexpr SkColor kBodyColor = SkColorSetRGB(0x75, 0x75, 0x75);

// FontList for the texts except for the header
gfx::FontList GetTextFontList() {
  gfx::Font default_font;
  int font_size_delta = kBodyTextFontSize - default_font.GetFontSize();
  gfx::Font font = default_font.Derive(font_size_delta, gfx::Font::NORMAL,
                                       gfx::Font::Weight::NORMAL);
  return gfx::FontList(font);
}

class ClickActivator : public ui::EventHandler {
 public:
  explicit ClickActivator(AdNotificationView* owner) : owner_(owner) {}
  ~ClickActivator() override = default;

 private:
  // ui::EventHandler
  void OnEvent(ui::Event* event) override {
    if (event->type() == ui::ET_MOUSE_PRESSED ||
        event->type() == ui::ET_GESTURE_TAP) {
      owner_->Activate();
    }
  }

  AdNotificationView* const owner_;

  ClickActivator(const ClickActivator&) = delete;
  ClickActivator& operator=(const ClickActivator&) = delete;
};

}  // namespace

void AdNotificationView::CreateContainerView(
    const Notification& notification) {
  CreateButtonsView(notification);

  CreateTitleView(notification);

  CreateBodyView(notification);

  container_view_->InvalidateLayout();
}

AdNotificationView::AdNotificationView(const Notification& notification)
    : NotificationView(notification) {
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, gfx::Insets(), 0));

  buttons_view_ = std::make_unique<NotificationButtonsView>(this);
  buttons_view_->set_owned_by_client();

  DCHECK(!title_view_);
  title_view_ = new NotificationTitleView();
  title_view_->AddChildView(buttons_view_.get());
  title_view_->SetBounds(0, 0, 30, 20);
  AddChildView(title_view_);

  container_view_ = new views::View();
  views::BoxLayout* layout_manager =
      container_view_->SetLayoutManager(std::make_unique<views::BoxLayout>(
          views::BoxLayout::Orientation::kHorizontal, kInsideBorderInsets));
  layout_manager->set_cross_axis_alignment(
      views::BoxLayout::CrossAxisAlignment::kStart);
  AddChildView(container_view_);

  body_view_ = new views::View();
  body_view_->SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kVertical, gfx::Insets(), 0));
  body_view_->SetBorder(views::CreateEmptyBorder(kBorderInsets));
  container_view_->AddChildView(body_view_);

  layout_manager->SetFlexForView(body_view_, 1);

  CreateForNotification(notification);

  SetNotifyEnterExitOnChild(true);

  click_activator_ = std::make_unique<ClickActivator>(this);
  AddPreTargetHandler(click_activator_.get());
}

AdNotificationView::~AdNotificationView() {
  RemovePreTargetHandler(click_activator_.get());
}

void AdNotificationView::Layout() {
  NotificationView::Layout();

  title_view_->Layout();
}

bool AdNotificationView::OnMousePressed(const ui::MouseEvent& event) {
  last_mouse_pressed_timestamp_ = base::TimeTicks(event.time_stamp());
  return true;
}

bool AdNotificationView::OnMouseDragged(const ui::MouseEvent& event) {
  return true;
}

void AdNotificationView::OnMouseReleased(const ui::MouseEvent& event) {
  if (!event.IsOnlyLeftMouseButton()) {
    return;
  }

  const std::string notification_id = GetNotificationId();
  NotificationWidget::OnClick(notification_id);

  NotificationView::OnMouseReleased(event);
}

void AdNotificationView::OnMouseEvent(ui::MouseEvent* event) {
  View::OnMouseEvent(event);
}

void AdNotificationView::CreateForNotification(
    const Notification& notification) {
  NotificationView::CreateForNotification(notification);

  CreateContainerView(notification);

  Layout();
  SchedulePaint();
}

void AdNotificationView::CreateButtonsView(
    const Notification& notification) {
  buttons_view_->ShowInfoButton(true);
  buttons_view_->ShowCloseButton(true);

  buttons_view_->ShowButtons(true);
}

void AdNotificationView::CreateTitleView(const Notification& notification) {
  title_view_->SetTitle(notification.title());
  title_view_->SetTitleElideBehavior(gfx::ELIDE_TAIL);
}

void AdNotificationView::CreateBodyView(
    const Notification& notification) {
  DCHECK(!body_label_);

  body_label_ = new views::Label(notification.message());

  const gfx::FontList& font_list = GetTextFontList();
  body_label_->SetFontList(font_list);

  body_label_->SetHorizontalAlignment(gfx::ALIGN_TO_HEAD);

  body_label_->SetEnabledColor(kBodyColor);
  body_label_->SetBackgroundColor(SK_ColorTRANSPARENT);

  body_label_->SetLineHeight(kLineHeight);
  body_label_->SetMaxLines(2);
  body_label_->SetMultiLine(true);

  body_label_->SetAllowCharacterBreak(true);

  const int width = kNotificationWidth - GetInsets().width();
  body_label_->SizeToFit(width);

  body_label_->SetVisible(true);

  body_view_->SetBorder(views::CreateEmptyBorder(kBorderInsets));

  body_view_->AddChildView(body_label_);
}

void AdNotificationView::Activate() {
  GetWidget()->widget_delegate()->SetCanActivate(true);
  GetWidget()->Activate();
}

}  // namespace brave_ads
