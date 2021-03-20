/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/ui/brave_ads/notification_buttons_view.h"

#include <memory>

#include "brave/app/vector_icons/vector_icons.h"
#include "brave/ui/brave_ads/notification_view.h"
#include "brave/ui/brave_ads/padded_image.h"
#include "brave/ui/brave_ads/padded_image_button.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/compositor/layer.h"
#include "ui/views/background.h"
#include "ui/views/layout/box_layout.h"

namespace brave_ads {

namespace {
constexpr SkColor kCloseButtonIconColor = SkColorSetRGB(0x4c, 0x36, 0xd2);
}  // namespace

NotificationButtonsView::NotificationButtonsView(
    NotificationView* notification_view)
    : notification_view_(notification_view) {
  DCHECK(notification_view_);

  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kHorizontal));

  // Use layer to change the opacity
  SetPaintToLayer();
  layer()->SetFillsBoundsOpaquely(false);

  SetBackground(views::CreateSolidBackground(SK_ColorTRANSPARENT));
}

NotificationButtonsView::~NotificationButtonsView() = default;

void NotificationButtonsView::ShowInfoButton(const bool show) {
  if (show && !info_button_) {
    info_button_ = AddChildView(std::make_unique<PaddedImage>());

    const gfx::ImageSkia image_skia =
        gfx::CreateVectorIcon(kBraveAdsInfoIcon, 35, SK_ColorTRANSPARENT);
    info_button_->SetImage(image_skia);

    // info_button_->SetAccessibleName(l10n_util::GetStringUTF16(
    //     IDS_MESSAGE_CENTER_NOTIFICATION_SNOOZE_BUTTON_TOOLTIP));

    // info_button_->SetTooltipText(l10n_util::GetStringUTF16(
    //     IDS_MESSAGE_CENTER_NOTIFICATION_SNOOZE_BUTTON_TOOLTIP));

    Layout();
  } else if (!show && info_button_) {
    DCHECK(Contains(info_button_));
    RemoveChildViewT(info_button_);
    info_button_ = nullptr;
  }
}

void NotificationButtonsView::ShowCloseButton(const bool show) {
  if (show && !close_button_) {
    close_button_ = AddChildView(std::make_unique<PaddedImageButton>(
        base::BindRepeating(&NotificationView::OnCloseButtonPressed,
                            base::Unretained(notification_view_))));

    const gfx::ImageSkia image_skia =
        gfx::CreateVectorIcon(kBraveAdsCloseButtonIcon, 18,
            kCloseButtonIconColor);
    close_button_->SetImage(views::Button::STATE_NORMAL, image_skia);

    // close_button_->SetAccessibleName(l10n_util::GetStringUTF16(
    //     IDS_MESSAGE_CENTER_NOTIFICATION_SNOOZE_BUTTON_TOOLTIP));

    // close_button_->SetTooltipText(l10n_util::GetStringUTF16(
    //     IDS_MESSAGE_CENTER_NOTIFICATION_SNOOZE_BUTTON_TOOLTIP));

    Layout();
  } else if (!show && close_button_) {
    DCHECK(Contains(close_button_));
    RemoveChildViewT(close_button_);
    close_button_ = nullptr;
  }
}

void NotificationButtonsView::ShowButtons(const bool show) {
  DCHECK(layer());
  // Manipulate the opacity instead of changing the visibility to keep the tab
  // order even when the view is invisible
  layer()->SetOpacity(show ? 1. : 0.);
  SetCanProcessEventsWithinSubtree(show);
}

}  // namespace brave_ads
