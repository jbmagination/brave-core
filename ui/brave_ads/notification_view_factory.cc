/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/ui/brave_ads/notification_view_factory.h"

#include "brave/ui/brave_ads/ad_notification_view.h"

namespace brave_ads {

// static
NotificationView* NotificationViewFactory::Create(
    const Notification& notification) {
  NotificationView* notification_view = new AdNotificationView(notification);
  return notification_view;
}

}  // namespace brave_ads
