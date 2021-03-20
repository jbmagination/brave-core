/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/ui/brave_ads/notification_title_view.h"

#include <memory>

#include "base/strings/string_number_conversions.h"
#include "brave/ui/brave_ads/public/cpp/constants.h"
#include "build/build_config.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/font_list.h"
#include "ui/views/controls/label.h"
#include "ui/views/layout/flex_layout.h"
#include "ui/views/layout/flex_layout_types.h"
#include "ui/views/painter.h"
#include "ui/views/view_class_properties.h"

namespace brave_ads {

namespace {

constexpr int kHeaderHeight = 24;

// The padding between controls in the header
constexpr gfx::Insets kHeaderSpacing(0, 0, 0, 0);

// The padding outer the header and the control buttons
constexpr gfx::Insets kHeaderOuterPadding(0, 0, 0, 0);

constexpr int kInnerHeaderHeight = kHeaderHeight - kHeaderOuterPadding.height();

constexpr gfx::Insets kTextViewPaddingDefault(9, 12, 6, 0);

constexpr int kHeaderTextFontSize = 14;

// Minimum spacing before the buttons
constexpr int kButtonSpacing = 10;

constexpr SkColor kTitleColor = SkColorSetRGB(0x75, 0x75, 0x75);

gfx::FontList GetHeaderTextFontList() {
  gfx::Font default_font;
  int font_size_delta = kHeaderTextFontSize - default_font.GetFontSize();
  gfx::Font font = default_font.Derive(font_size_delta, gfx::Font::NORMAL,
                                       gfx::Font::Weight::SEMIBOLD);
  return gfx::FontList(font);
}

gfx::Insets CalculateTopPadding(int font_list_height) {
#if defined(OS_WIN)
  // On Windows, the fonts can have slightly different metrics reported,
  // depending on where the code runs. In Chrome, DirectWrite is on, which means
  // font metrics are reported from Skia, which rounds from float using ceil.
  // In unit tests, however, GDI is used to report metrics, and the height
  // reported there is consistent with other platforms. This means there is a
  // difference of 1px in height between Chrome on Windows and everything else
  // (where everything else includes unit tests on Windows). This 1px causes the
  // text and everything else to stop aligning correctly, so we account for it
  // by shrinking the top padding by 1
  if (font_list_height != 15) {
    return kTextViewPaddingDefault - gfx::Insets(1 /* top */, 0, 0, 0);
  }
#endif

  return kTextViewPaddingDefault;
}

}  // namespace

NotificationTitleView::NotificationTitleView(PressedCallback callback)
    : views::Button(callback) {
  const views::FlexSpecification kAppNameFlex =
      views::FlexSpecification(views::MinimumFlexSizeRule::kScaleToZero,
                               views::MaximumFlexSizeRule::kPreferred)
          .WithOrder(1);

  const views::FlexSpecification kSpacerFlex =
      views::FlexSpecification(views::MinimumFlexSizeRule::kScaleToMinimum,
                               views::MaximumFlexSizeRule::kUnbounded)
          .WithOrder(2);

  auto* layout = SetLayoutManager(std::make_unique<views::FlexLayout>());
  layout->SetDefault(views::kMarginsKey, kHeaderSpacing);
  layout->SetInteriorMargin(kHeaderOuterPadding);
  layout->SetCollapseMargins(true);

  // Font list for text views
  gfx::FontList font_list = GetHeaderTextFontList();
  const int font_list_height = font_list.GetHeight();
  gfx::Insets text_view_padding(CalculateTopPadding(font_list_height));

  auto create_label = [&font_list, font_list_height, text_view_padding]() {
    auto* label = new views::Label();
    label->SetFontList(font_list);
    label->SetLineHeight(font_list_height);
    label->SetHorizontalAlignment(gfx::ALIGN_LEFT);
    label->SetBorder(views::CreateEmptyBorder(text_view_padding));
    return label;
  };

  // Title view
  title_label_ = create_label();
  title_label_->SetMultiLine(false);
  title_label_->SetEnabledColor(kTitleColor);
  title_label_->SetBackgroundColor(SK_ColorTRANSPARENT);
  title_label_->SetProperty(views::kFlexBehaviorKey, kAppNameFlex);
  AddChildView(title_label_);

  // Spacer between left-aligned views and right-aligned views
  views::View* spacer = new views::View;
  spacer->SetPreferredSize(gfx::Size(kButtonSpacing, kInnerHeaderHeight));
  spacer->SetProperty(views::kFlexBehaviorKey, kSpacerFlex);
  AddChildView(spacer);

  SetPreferredSize(gfx::Size(kNotificationWidth, kHeaderHeight));
}

NotificationTitleView::~NotificationTitleView() = default;

void NotificationTitleView::SetTitle(const base::string16& text) {
  DCHECK(title_label_);

  title_label_->SetText(text);
}

void NotificationTitleView::SetTitleElideBehavior(
    gfx::ElideBehavior elide_behavior) {
  DCHECK(title_label_);

  title_label_->SetElideBehavior(elide_behavior);
}

}  // namespace brave_ads
