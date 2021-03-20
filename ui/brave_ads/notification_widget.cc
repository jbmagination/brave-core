/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/ui/brave_ads/notification_widget.h"

#include <map>
#include <memory>
#include <utility>

#include "base/bind.h"
#include "brave/ui/brave_ads/notification_view.h"
#include "brave/ui/brave_ads/notification_view_factory.h"
#include "brave/ui/brave_ads/public/cpp/constants.h"
#include "brave/ui/brave_ads/public/cpp/notification.h"
#include "build/build_config.h"
#include "ui/display/display.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/widget/widget.h"

#if defined(OS_WIN)
#include "ui/views/widget/desktop_aura/desktop_native_widget_aura.h"
#endif

namespace brave_ads {

namespace {

static std::map<std::string, NotificationWidget*> g_notification_widgets;

const int kWidgetPadding = 10;

}  // namespace

// static
void NotificationWidget::Show(const Notification& notification) {
  const std::string& notification_id = notification.id();

  DCHECK(!g_notification_widgets[notification_id]);
  g_notification_widgets[notification_id] =
          new NotificationWidget(notification);
}

// static
void NotificationWidget::Close(const std::string& notification_id,
                               const bool by_user) {
  DCHECK(!notification_id.empty());

  NotificationWidget* notification_widget =
      g_notification_widgets[notification_id];
  DCHECK(notification_widget);

  g_notification_widgets.erase(notification_id);

  NotificationDelegate* notification_delegate =
      notification_widget->notification_.delegate();
  if (notification_delegate) {
    notification_delegate->OnClose(by_user);
  }

  NotificationView* notification_view =
      notification_widget->GetNotificationView();
  DCHECK(notification_view);

  // Destroy the widget when done. The observer deletes itself on completion
  notification_view->FadeOut(base::BindOnce([](
      const std::string& notification_id) {
    DCHECK(!notification_id.empty());

    NotificationWidget* notification_widget =
        g_notification_widgets[notification_id];
    DCHECK(notification_widget);

    notification_widget->CloseWidgetViewForId(notification_id);
  }, notification_id));
}

// static
void NotificationWidget::OnClick(const std::string& notification_id) {
  DCHECK(!notification_id.empty());

  NotificationWidget* notification_widget =
      g_notification_widgets[notification_id];
  DCHECK(notification_widget);

  NotificationDelegate* delegate =
      notification_widget->notification_.delegate();
  if (delegate) {
    delegate->OnClick(base::nullopt, base::nullopt);
  }

  Close(notification_id, /* by_user */ true);
}

NotificationWidget::NotificationWidget(const Notification& notification)
    : notification_(notification) {
  SetLayoutManager(std::make_unique<views::FillLayout>());

  CreateWidget();

  // TODO(tmancey): Is this needed?
  SetNotifyEnterExitOnChild(true);
}

NotificationWidget::~NotificationWidget() = default;

const char* NotificationWidget::GetClassName() const {
  return "NotificationWidget";
}

///////////////////////////////////////////////////////////////////////////////

void NotificationWidget::CreateWidget() {
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_POPUP);
  params.z_order = ui::ZOrderLevel::kFloatingWindow;
  params.type = views::Widget::InitParams::TYPE_WINDOW_FRAMELESS;
  params.delegate = this;
  params.name = "BraveAdsNotificationWidget";
  params.opacity = views::Widget::InitParams::WindowOpacity::kTranslucent;
  params.bounds = GetBounds();

#if defined(OS_WIN)
  // We want to ensure that this toast always goes to the native desktop,
  // not the Ash desktop (since there is already another toast contents view
  // there.
  if (!params.parent) {
    DCHECK(!params.native_widget);
    params.native_widget = new views::DesktopNativeWidgetAura(widget_view_);
  }
#endif

  DCHECK(!widget_view_);
  widget_view_ = new views::Widget();
  widget_view_->Init(std::move(params));

  notification_view_ = NotificationViewFactory::Create(notification_);
  widget_view_->SetContentsView(notification_view_);
  widget_view_->ShowInactive();

  observer_.Add(widget_view_);
}

gfx::Rect NotificationWidget::GetBounds() const {
  const gfx::Rect visible_frame = GetVisibleFrameForPrimaryDisplay();

  const int x = visible_frame.right() - (kNotificationWidth + kWidgetPadding);

#if defined(OS_WIN)
  const int y = visible_frame.bottom() - (kNotificationHeight + kWidgetPadding);
#else
  const int y = visible_frame.y() + kWidgetPadding;
#endif

  return gfx::Rect(x, y, kNotificationWidth, kNotificationHeight);
}

#if !defined(OS_WIN) && !defined(OS_MAC)
gfx::Rect NotificationWidget::GetVisibleFrameForPrimaryDisplay() const {
  return gfx::Rect(display::Screen::GetScreen()->GetPrimaryDisplay().size());
}
#endif

void NotificationWidget::CloseWidgetViewForId(
    const std::string& notification_id) const {
  DCHECK(!notification_id.empty());
  DCHECK(widget_view_);

  widget_view_->CloseNow();
}

void NotificationWidget::OnWidgetDestroyed(views::Widget* widget) {
  DCHECK(widget);

  observer_.Remove(widget);
}

}  // namespace brave_ads
