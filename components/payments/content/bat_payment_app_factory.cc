/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/payments/content/bat_payment_app_factory.h"

#include <vector>

#include "base/feature_list.h"
#include "brave/components/payments/content/bat_payment_app.h"
#include "components/payments/content/content_payment_request_delegate.h"
#include "components/payments/content/payment_request_spec.h"
#include "components/payments/core/features.h"

namespace payments {

BatPaymentAppFactory::BatPaymentAppFactory()
    : PaymentAppFactory(PaymentApp::Type::AUTOFILL) {}

BatPaymentAppFactory::~BatPaymentAppFactory() = default;

void BatPaymentAppFactory::Create(base::WeakPtr<Delegate> delegate) {
  DCHECK(delegate);

  auto spec = delegate->GetSpec();
  if (!spec)
    return;

  if (spec->stringified_method_data().count("bat") > 0) {
    auto app = std::make_unique<BatPaymentApp>(spec);
    if (app) {
      delegate->OnPaymentAppCreated(std::move(app));
    }
  }

  delegate->OnDoneCreatingPaymentApps();
}

}  // namespace payments
