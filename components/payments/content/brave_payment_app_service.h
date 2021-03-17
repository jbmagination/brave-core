/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_PAYMENTS_CONTENT_BRAVE_PAYMENT_APP_SERVICE_H_
#define BRAVE_COMPONENTS_PAYMENTS_CONTENT_BRAVE_PAYMENT_APP_SERVICE_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "brave/components/payments/content/bat_payment_app_factory.h"
#include "components/keyed_service/core/keyed_service.h"
#include "components/payments/content/payment_app_factory.h"
#include "components/payments/content/payment_app_service.h"

namespace payments {

class BravePaymentAppService : public PaymentAppService {
 public:
  explicit BravePaymentAppService(content::BrowserContext* context);
  ~BravePaymentAppService() override;
  void Create(base::WeakPtr<PaymentAppFactory::Delegate> delegate);
  size_t GetNumberOfFactories() const;

 private:
  std::vector<std::unique_ptr<PaymentAppFactory>> factories_;

  DISALLOW_COPY_AND_ASSIGN(BravePaymentAppService);
};

}  // namespace payments

#endif  // BRAVE_COMPONENTS_PAYMENTS_CONTENT_BRAVE_PAYMENT_APP_SERVICE_H_
