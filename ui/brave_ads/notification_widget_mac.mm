/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#import "brave/ui/brave_ads/notification_widget.h"

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>

#import "ui/gfx/mac/coordinate_conversion.h"

namespace brave_ads {

gfx::Rect NotificationWidget::GetVisibleFrameForPrimaryDisplay() const {
  const NSRect visibleFrame = [[NSScreen mainScreen] visibleFrame];

  return gfx::ScreenRectFromNSRect(visibleFrame);
}

}  // namespace brave_ads
