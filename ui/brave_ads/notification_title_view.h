/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_ADS_NOTIFICATION_TITLE_VIEW_H_
#define BRAVE_UI_BRAVE_ADS_NOTIFICATION_TITLE_VIEW_H_

#include "base/macros.h"
#include "ui/gfx/text_constants.h"
#include "ui/views/controls/button/button.h"

namespace views {
class ImageView;
class Label;
}  // namespace views

namespace brave_ads {

class NotificationTitleView : public views::Button {
 public:
  explicit NotificationTitleView(PressedCallback callback = PressedCallback());
  ~NotificationTitleView() override;

  void SetTitle(const base::string16& name);
  void SetTitleElideBehavior(gfx::ElideBehavior elide_behavior);

 private:
  views::Label* title_label_ = nullptr;

  NotificationTitleView(const NotificationTitleView&) = delete;
  NotificationTitleView& operator=(const NotificationTitleView&) = delete;
};

}  // namespace brave_ads

#endif  // BRAVE_UI_BRAVE_ADS_NOTIFICATION_TITLE_VIEW_H_
