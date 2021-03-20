/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_ADS_PADDED_IMAGE_H_
#define BRAVE_UI_BRAVE_ADS_PADDED_IMAGE_H_

#include "ui/views/controls/image_view.h"

namespace brave_ads {

class PaddedImage : public views::ImageView {
 public:
  PaddedImage();
  ~PaddedImage() override = default;

 private:
  PaddedImage(const PaddedImage&) = delete;
  PaddedImage& operator=(const PaddedImage&) = delete;
};

}  // namespace brave_ads

#endif  // BRAVE_UI_BRAVE_ADS_PADDED_IMAGE_H_
