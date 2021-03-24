/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "chrome/browser/ui/views/payments/payment_request_dialog_view.h"

#include "brave/browser/brave_rewards/checkout_dialog.cc"
#include "brave/components/brave_rewards/common/constants.h"
#include "components/payments/content/payment_request.h"

namespace payments {

namespace {

bool IsBatSupportedMethod(base::WeakPtr<PaymentRequest> request) {
  auto spec = request->spec();
  for (const auto& data : spec->method_data()) {
    if (data->supported_method == brave_rewards::kBatPaymentMethod) {
      return true;
    }
  }
  return false;
}

}  // namespace

void PaymentRequestDialogView::CloseDialog() {
  if (IsBatSupportedMethod(request_)) {
    return;
  }
  PaymentRequestDialogView::CloseDialog_ChromiumImpl();
}

void PaymentRequestDialogView::ShowDialog() {
  if (IsBatSupportedMethod(request_)) {
    brave_rewards::ShowCheckoutDialog(request_->web_contents(), request_);
    return;
  }
  PaymentRequestDialogView::ShowDialog_ChromiumImpl();
}

}  // namespace payments

#define ShowDialog ShowDialog_ChromiumImpl
#define CloseDialog CloseDialog_ChromiumImpl
#include "../../../../../../../chrome/browser/ui/views/payments/payment_request_dialog_view.cc"  // NOLINT
#undef CloseDialog
#undef ShowDialog
