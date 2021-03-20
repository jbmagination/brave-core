/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_ADS_NOTIFICATION_WIDGET_H_
#define BRAVE_UI_BRAVE_ADS_NOTIFICATION_WIDGET_H_

#include <string>

#include "base/scoped_observer.h"
#include "brave/ui/brave_ads/public/cpp/notification.h"
#include "ui/views/widget/widget.h"
#include "ui/views/widget/widget_delegate.h"
#include "ui/views/widget/widget_observer.h"

namespace brave_ads {

class NotificationView;

class NotificationWidget : public views::WidgetDelegateView,
                           public views::WidgetObserver {
 public:
  explicit NotificationWidget(const Notification& notification);
  ~NotificationWidget() override;

  const char* GetClassName() const override;

  // Show the notification widget for the given |notification|
  static void Show(const Notification& notification);

  // Close the notification widget for the given |notification_id|. |by_user|
  // is true if the notification widget was closed by the user, otherwise false
  static void Close(const std::string& notification_id, const bool by_user);

  // User clicked the notification widget for the given |notification_id|
  static void OnClick(const std::string& notification_id);

 private:
  Notification notification_;

  views::Widget* widget_view_ = nullptr;

  NotificationView* notification_view_ = nullptr;  // NOT OWNED

  gfx::Rect GetVisibleFrameForPrimaryDisplay() const;

  void CreateWidget();

  gfx::Rect GetBounds() const;

  NotificationView* GetNotificationView() const { return notification_view_; }

  void CloseWidgetViewForId(const std::string& notification_id) const;

  ScopedObserver<views::Widget, views::WidgetObserver> observer_{this};

  // views::WidgetObserver implementation
  void OnWidgetDestroyed(views::Widget* widget) override;

  NotificationWidget(const NotificationWidget&) = delete;
  NotificationWidget& operator=(const NotificationWidget&) = delete;
};

}  // namespace brave_ads

#endif  // BRAVE_UI_BRAVE_ADS_NOTIFICATION_WIDGET_H_
