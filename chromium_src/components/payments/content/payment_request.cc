/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "components/payments/content/payment_request.h"

#include "brave/components/brave_rewards/common/constants.h"
#include "third_party/blink/public/mojom/payments/payment_request.mojom.h"

using payments::mojom::PaymentErrorReason;

#define Init Init_ChromiumImpl
#include "../../../../../components/payments/content/payment_request.cc"
#undef Init

namespace payments {

void PaymentRequest::TerminateConnectionWithMessage(PaymentErrorReason reason,
                                                    std::string err) {
  if (!err.empty()) {
    client_->OnError(
        reason,
        err);
  }
  TerminateConnection();
}

void PaymentRequest::Init(
    mojo::PendingRemote<mojom::PaymentRequestClient> client,
    std::vector<mojom::PaymentMethodDataPtr> method_data,
    mojom::PaymentDetailsPtr details,
    mojom::PaymentOptionsPtr options) {
  bool result = false;
  for (size_t i = 0; i < method_data.size(); i++) {
    if (method_data[i]->supported_method == brave_rewards::kBatPaymentMethod) {
      result = true;
      break;
    }
  }

  if (result) {
    if (details->display_items.has_value()) {
      for (const mojom::PaymentItemPtr& display_item : *details->display_items) {
        if (!(display_item->sku.has_value())) {
          log_.Error(brave_rewards::errors::kMissingSKUTokens);
          TerminateConnection();
          return;
        }
      }
    }
  }

  Init_ChromiumImpl(std::move(client),
                    std::move(method_data),
                    std::move(details),
                    std::move(options));  
}

}