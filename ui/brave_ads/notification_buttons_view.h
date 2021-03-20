/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_ADS_NOTIFICATION_BUTTONS_VIEW_H_
#define BRAVE_UI_BRAVE_ADS_NOTIFICATION_BUTTONS_VIEW_H_

#include "ui/views/view.h"

namespace brave_ads {

class NotificationView;
class PaddedImage;
class PaddedImageButton;

class NotificationButtonsView : public views::View {
 public:
  explicit NotificationButtonsView(NotificationView* message_view);

  ~NotificationButtonsView() override;

  void ShowInfoButton(const bool show);
  void ShowCloseButton(const bool show);
  void ShowButtons(const bool show);

 private:
  NotificationView* notification_view_ = nullptr;

  PaddedImage* info_button_;
  PaddedImageButton* close_button_;

  NotificationButtonsView(const NotificationButtonsView&) = delete;
  NotificationButtonsView& operator=(const NotificationButtonsView&) = delete;
};

}  // namespace brave_ads

#endif  // BRAVE_UI_BRAVE_ADS_NOTIFICATION_BUTTONS_VIEW_H_
