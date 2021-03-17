/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/payments/content/brave_payment_app_service.h"

#include <cstddef>
#include <memory>
#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/payments/content/bat_payment_app_factory.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/payments/content/autofill_payment_app_factory.h"
#include "components/payments/content/payment_app_factory.h"
#include "components/payments/content/payment_app_service.h"
#include "components/payments/content/service_worker_payment_app_factory.h"

namespace payments {

//To-Do: Refactor
BravePaymentAppService::BravePaymentAppService(content::BrowserContext* context) :
	PaymentAppService(context) {
    factories_.emplace_back(std::make_unique<AutofillPaymentAppFactory>());
    factories_.push_back(std::make_unique<ServiceWorkerPaymentAppFactory>());
    factories_.emplace_back(std::make_unique<BatPaymentAppFactory>());
}

BravePaymentAppService::~BravePaymentAppService() = default;

size_t BravePaymentAppService::GetNumberOfFactories() const {
  return factories_.size();
}

void BravePaymentAppService::Create(
    base::WeakPtr<PaymentAppFactory::Delegate> delegate) {
  for (const auto& factory : factories_) {
    factory->Create(delegate);
  }
}

}  // namespace payments
