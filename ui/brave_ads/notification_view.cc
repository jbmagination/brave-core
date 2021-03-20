/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/ui/brave_ads/notification_view.h"

#include "brave/ui/brave_ads/notification_widget.h"
#include "brave/ui/brave_ads/notification_background_painter.h"
#include "brave/ui/brave_ads/public/cpp/constants.h"
#include "build/build_config.h"
#include "third_party/skia/include/core/SkColor.h"
#include "ui/compositor/closure_animation_observer.h"
#include "ui/compositor/scoped_layer_animation_settings.h"
#include "ui/gfx/shadow_util.h"
#include "ui/gfx/shadow_value.h"
#include "ui/views/background.h"
#include "ui/views/border.h"

namespace brave_ads {

namespace {

const float kFadeInDurationMs = 250.0f;
const float kFadeOutDurationMs = 175.0f;

const int kCornerRadius = 7;

constexpr SkColor kBackgroundColor = SkColorSetRGB(0xed, 0xf0, 0xf2);

const int kWindowsShadowElevation = 2;
const int kWindowsShadowRadius = 0;

bool ShouldShowAeroShadowBorder() {
#if defined(OS_WIN)
  return ui::win::IsAeroGlassEnabled();
#else
  return false;
#endif
}

}  // namespace

NotificationView::NotificationView(const Notification& notification)
    : notification_id_(notification.id()) {
  SetFocusBehavior(FocusBehavior::ALWAYS);

  SetPaintToLayer();
  layer()->SetFillsBoundsOpaquely(false);

  layer()->SetOpacity(0.0);

  CreateForNotification(notification);

  // If Aero is enabled, set shadow border
  if (ShouldShowAeroShadowBorder()) {
    const auto& shadow =
        gfx::ShadowDetails::Get(kWindowsShadowElevation, kWindowsShadowRadius);

    gfx::Insets insets = gfx::ShadowValue::GetBlurRegion(shadow.values);

    SetBorder(views::CreateBorderPainter(
        views::Painter::CreateImagePainter(shadow.ninebox_image, insets),
        -gfx::ShadowValue::GetMargin(shadow.values)));
  }

  FadeIn();
}

NotificationView::~NotificationView() = default;

void NotificationView::CreateForNotification(const Notification& notification) {
  ApplyCornerRadius();
}

void NotificationView::ApplyCornerRadius() {
  SetBackground(views::CreateBackgroundFromPainter(
      std::make_unique<NotificationBackgroundPainter>(kCornerRadius,
                                                      kCornerRadius,
                                                      kBackgroundColor)));

  SchedulePaint();
}

void NotificationView::OnPaint(gfx::Canvas* canvas) {
  if (ShouldShowAeroShadowBorder()) {
    // If the border is shadow, paint border first
    OnPaintBorder(canvas);
    // Clip at the border so we don't paint over it
    canvas->ClipRect(GetContentsBounds());
    OnPaintBackground(canvas);
  } else {
    views::View::OnPaint(canvas);
  }
}

void NotificationView::OnThemeChanged() {
  InkDropHostView::OnThemeChanged();
}

void NotificationView::OnCloseButtonPressed() {
  if (is_closing_) {
    return;
  }

  is_closing_ = true;

  NotificationWidget::Close(notification_id_, true);
}

void NotificationView::FadeIn() {
  ui::ScopedLayerAnimationSettings animation(layer()->GetAnimator());
  animation.SetTransitionDuration(
      base::TimeDelta::FromMilliseconds(kFadeInDurationMs));
  animation.SetTweenType(gfx::Tween::Type::EASE_IN);
  layer()->SetOpacity(1.0);
}

void NotificationView::FadeOut(base::OnceClosure closure) {
  ui::ScopedLayerAnimationSettings animation(layer()->GetAnimator());
  animation.SetTransitionDuration(
      base::TimeDelta::FromMilliseconds(kFadeOutDurationMs));
  animation.SetTweenType(gfx::Tween::Type::EASE_IN);
  layer()->SetOpacity(0.0);

  animation.AddObserver(new ui::ClosureAnimationObserver(std::move(closure)));
}

}  // namespace brave_ads
