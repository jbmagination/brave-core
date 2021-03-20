/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_ADS_AD_NOTIFICATION_VIEW_H_
#define BRAVE_UI_BRAVE_ADS_AD_NOTIFICATION_VIEW_H_

#include <memory>

#include "base/time/time.h"
#include "brave/ui/brave_ads/notification_view.h"
#include "ui/views/controls/label.h"

namespace views {
class Label;
}

namespace brave_ads {

class NotificationTitleView;
class NotificationButtonsView;

// View that displays all current types of notification (web, basic, image, and
// list) except the custom notification. Future notification types may be
// handled by other classes, in which case instances of those classes would be
// returned by the Create() factory method below
class AdNotificationView : public NotificationView {
 public:
  explicit AdNotificationView(const Notification& notification);
  ~AdNotificationView() override;

  void Activate();

  void Layout() override;

  void CreateForNotification(const Notification& notification) override;

  bool OnMousePressed(const ui::MouseEvent& event) override;
  bool OnMouseDragged(const ui::MouseEvent& event) override;
  void OnMouseReleased(const ui::MouseEvent& event) override;
  void OnMouseEvent(ui::MouseEvent* event) override;

 private:
  void CreateButtonsView(const Notification& notification);

  views::View* container_view_ = nullptr;
  void CreateContainerView(const Notification& notification);

  std::unique_ptr<NotificationButtonsView> buttons_view_;

  NotificationTitleView* title_view_ = nullptr;
  void CreateTitleView(const Notification& notification);

  views::View* body_view_ = nullptr;
  views::Label* body_label_ = nullptr;
  void CreateBodyView(const Notification& notification);

  std::unique_ptr<ui::EventHandler> click_activator_;

  base::TimeTicks last_mouse_pressed_timestamp_;

  base::WeakPtrFactory<AdNotificationView> weak_ptr_factory_{this};

  AdNotificationView(const AdNotificationView&) = delete;
  AdNotificationView& operator=(const AdNotificationView&) = delete;
};

}  // namespace brave_ads

#endif  // BRAVE_UI_BRAVE_ADS_AD_NOTIFICATION_VIEW_H_
