/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_ADS_NOTIFICATION_VIEW_H_
#define BRAVE_UI_BRAVE_ADS_NOTIFICATION_VIEW_H_

#include <memory>
#include <string>

#include "base/callback_forward.h"
#include "base/macros.h"
#include "brave/ui/brave_ads/public/cpp/notification.h"
#include "brave/ui/brave_ads/public/cpp/notification_delegate.h"
#include "ui/compositor/layer_animation_observer.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/geometry/insets.h"
#include "ui/gfx/image/image.h"
#include "ui/views/animation/ink_drop_host_view.h"
#include "ui/views/view.h"

namespace brave_ads {

class Notification;

class NotificationView : public views::InkDropHostView {
 public:
  explicit NotificationView(const Notification& notification);

  ~NotificationView() override;

  virtual void CreateForNotification(const Notification& notification);

  void ApplyCornerRadius();

  void FadeOut(base::OnceClosure closure);

  std::string GetNotificationId() const { return notification_id_; }

  void OnCloseButtonPressed();

  void OnPaint(gfx::Canvas* canvas) override;

  void OnThemeChanged() override;

 private:
  std::string notification_id_;

  bool is_closing_ = false;

  void FadeIn();

  NotificationView(const NotificationView&) = delete;
  NotificationView& operator=(const NotificationView&) = delete;
};

}  // namespace brave_ads

#endif  // BRAVE_UI_BRAVE_ADS_NOTIFICATION_VIEW_H_
