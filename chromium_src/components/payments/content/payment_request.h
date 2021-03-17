/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_CHROMIUM_SRC_COMPONENTS_PAYMENTS_CONTENT_PAYMENT_REQUEST_H_
#define BRAVE_CHROMIUM_SRC_COMPONENTS_PAYMENTS_CONTENT_PAYMENT_REQUEST_H_

#include <memory>
#include <vector>

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "components/payments/content/developer_console_logger.h"
#include "components/payments/content/initialization_task.h"
#include "components/payments/content/payment_handler_host.h"
#include "components/payments/content/payment_request_display_manager.h"
#include "components/payments/content/payment_request_spec.h"
#include "components/payments/content/payment_request_state.h"
#include "components/payments/content/service_worker_payment_app.h"
#include "components/payments/core/journey_logger.h"
#include "content/public/browser/global_routing_id.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/blink/public/mojom/payments/payment_request.mojom.h"
#include "url/gurl.h"
#include "url/origin.h"

using payments::mojom::PaymentErrorReason;

#define BRAVE_TERMINATE_CONNECTION_WITH_MESSAGE                                   \
  void TerminateConnectionWithMessage(payments::mojom::PaymentErrorReason reason, \
  	                  std::string err);

#define Init \
    Init_ChromiumImpl(mojo::PendingRemote<mojom::PaymentRequestClient> client, \
    	              std::vector<mojom::PaymentMethodDataPtr> method_data,    \
                      mojom::PaymentDetailsPtr details,                        \
                      mojom::PaymentOptionsPtr options);                       \
    void Init
#include "../../../../../components/payments/content/payment_request.h"
#undef Init
#undef BRAVE_TERMINATE_CONNECTION_WITH_MESSAGE

#endif  // BRAVE_CHROMIUM_SRC_COMPONENTS_PAYMENTS_CONTENT_PAYMENT_REQUEST_H_