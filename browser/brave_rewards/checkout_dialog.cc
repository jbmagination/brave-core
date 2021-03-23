/* Copyright 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/brave_rewards/checkout_dialog.h"

#include <memory>
#include <vector>
#include <utility>

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/task/post_task.h"
#include "base/values.h"
#include "brave/browser/brave_rewards/rewards_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service_impl.h"
#include "brave/components/brave_rewards/common/constants.h"
#include "brave/common/webui_url_constants.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "chrome/browser/ui/webui/constrained_web_dialog_ui.h"
#include "components/payments/content/payment_request.h"
#include "components/payments/core/payer_data.h"
#include "components/web_modal/web_contents_modal_dialog_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/browser_task_traits.h"
#include "third_party/blink/public/mojom/payments/payment_request.mojom.h"
#include "ui/web_dialogs/web_dialog_delegate.h"

using content::WebContents;
using content::WebUIMessageHandler;
using payments::PaymentRequest;
using payments::mojom::PaymentErrorReason;

namespace brave_rewards {

enum DialogCloseReason {
  Complete,
  InsufficientBalance,
  UnverifiedWallet,
  UserCancelled
};

constexpr int kDialogMinWidth = 548;  // 490;
constexpr int kDialogMinHeight = 200;

constexpr int kDialogMaxWidth = 548;
constexpr int kDialogMaxHeight = 800;

CheckoutDialogDelegate::CheckoutDialogDelegate(base::Value params,
                                               base::WeakPtr<PaymentRequest> request)
    : params_(std::move(params)),
      request_(request) {}

CheckoutDialogDelegate::~CheckoutDialogDelegate() = default;

ui::ModalType CheckoutDialogDelegate::GetDialogModalType() const {
  // Not used, returning dummy value.
  NOTREACHED();
  return ui::MODAL_TYPE_WINDOW;
}

base::string16 CheckoutDialogDelegate::GetDialogTitle() const {
  return base::string16();
}

GURL CheckoutDialogDelegate::GetDialogContentURL() const {
  return GURL(kBraveUICheckoutURL);
}

void CheckoutDialogDelegate::GetWebUIMessageHandlers(
    std::vector<WebUIMessageHandler*>* handlers) const {
  // BraveCheckoutUI will add message handlers.
  handlers->push_back(new CheckoutDialogHandler(request_));
}

void CheckoutDialogDelegate::GetDialogSize(gfx::Size* size) const {
  // TODO(zenparsing): Is the constrained modal dialog
  // really what we want? It is designed for interfaces
  // that are fixed size for a given screen size.
}

std::string CheckoutDialogDelegate::GetDialogArgs() const {
  std::string json;
  base::JSONWriter::Write(params_, &json);
  return json;
}

void CheckoutDialogDelegate::OnDialogClosed(const std::string& result) {
  int reason;
  if (!base::StringToInt(result, &reason)) {
    request_->TerminateConnectionWithMessage(PaymentErrorReason::UNKNOWN,
                                             errors::kInvalidReturnValue);
    return;
  }
  if (reason == DialogCloseReason::UserCancelled) {
    request_->TerminateConnectionWithMessage(PaymentErrorReason::USER_CANCEL,
                                             errors::kTransactionCancelled);
    return;
  }
  if (reason == DialogCloseReason::UnverifiedWallet) {
    request_->TerminateConnectionWithMessage(PaymentErrorReason::NOT_SUPPORTED,
                                             errors::kUnverifiedUserWallet);
    return;
  }
  if (reason == DialogCloseReason::InsufficientBalance) {
    request_->TerminateConnectionWithMessage(PaymentErrorReason::NOT_SUPPORTED,
                                             errors::kInsufficientBalance);
    return;
  }
  return;
}

void CheckoutDialogDelegate::OnCloseContents(WebContents* source,
                                             bool* out_close_dialog) {
  *out_close_dialog = true;
}

bool CheckoutDialogDelegate::ShouldShowDialogTitle() const {
  return false;
}

CheckoutDialogHandler::CheckoutDialogHandler(base::WeakPtr<PaymentRequest> request)
    : request_(request) {}

CheckoutDialogHandler::~CheckoutDialogHandler() = default;

// Overridden from WebUIMessageHandler
void CheckoutDialogHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "paymentRequestComplete",
      base::BindRepeating(&CheckoutDialogHandler::HandlePaymentCompletion,
                          base::Unretained(this)));
}

void CheckoutDialogHandler::OnSKUProcessed(const ledger::type::Result result, const std::string& value) {
  if (result == ledger::type::Result::LEDGER_OK) {
    payments::mojom::PaymentDetailsPtr details = payments::mojom::PaymentDetails::New();
    details->id = value;
    request_->spec()->UpdateWith(std::move(details));
    request_->Pay();
    return;
  }
  request_->TerminateConnectionWithMessage(PaymentErrorReason::UNKNOWN,
                                           errors::kBatTransactionFailed);
}

void CheckoutDialogHandler::ProcessSKU() {
  auto spec = request_->spec();
  if (!spec || spec->stringified_method_data().count(kBatPaymentMethod) < 0) {
    request_->TerminateConnectionWithMessage(PaymentErrorReason::INVALID_DATA_FROM_RENDERER,
                                             errors::kInvalidData);
    return;
  }

  auto* rfh = content::RenderFrameHost::FromID(request_->initiator_frame_routing_id());
  if (!rfh){
    request_->TerminateConnectionWithMessage(PaymentErrorReason::INVALID_DATA_FROM_RENDERER,
                                             errors::kInvalidRenderer);
    return;
  }

  auto* rewards_service = brave_rewards::RewardsServiceFactory::GetForProfile(
    Profile::FromBrowserContext(rfh->GetBrowserContext()));
  if (!rewards_service) {
    request_->TerminateConnectionWithMessage(PaymentErrorReason::INVALID_DATA_FROM_RENDERER,
                                             errors::kRewardsNotInitialized);
    return;
  }

  const auto& display_items = spec->GetDisplayItems(request_->state()->selected_app());
  for (size_t i = 0; i < display_items.size(); i++) {
    DCHECK((*display_items[i])->sku.has_value());
    auto item = ledger::type::SKUOrderItem::New();

    item->sku = (*display_items[i])->sku.value();
    item->quantity = 1;
    item->type = ledger::type::SKUOrderItemType::SINGLE_USE;
    items_.push_back(std::move(item));
  }

  auto callback = base::BindOnce(&CheckoutDialogHandler::OnSKUProcessed,
                                 base::Unretained(this));

  rewards_service->ProcessSKU(std::move(items_),
                              ledger::constant::kWalletUphold,
                              std::move(callback));
}

void CheckoutDialogHandler::HandlePaymentCompletion(
    const base::ListValue* args) {
    if(!request_->spec()->IsInitialized()) {
      return;
    }
    this->ProcessSKU();
}

void PostPaymentRequestErrors(base::WeakPtr<PaymentRequest> request,
                              PaymentErrorReason reason,
                              std::string err) {
  request->TerminateConnectionWithMessage(reason, err);
}

void GetPublisherDetailsCallback(
    WebContents* initiator,
    base::WeakPtr<PaymentRequest> request,
    const ledger::type::Result result,
    ledger::type::PublisherInfoPtr info) {
  double total;
  if (!info) {
    request->TerminateConnectionWithMessage(PaymentErrorReason::UNKNOWN,
                                            errors::kInvalidPublisher);
    return;
  }

  if (info->status ==  ledger::type::PublisherStatus::NOT_VERIFIED) {
    request->TerminateConnectionWithMessage(PaymentErrorReason::NOT_SUPPORTED_FOR_INVALID_ORIGIN_OR_SSL,
                                            errors::kInvalidPublisher);
    return;
  }

  base::WeakPtr<payments::PaymentRequestSpec> spec = request->spec();
  if (!spec) {
    request->TerminateConnectionWithMessage(PaymentErrorReason::INVALID_DATA_FROM_RENDERER,
                                            errors::kInvalidData);
    return;
  }

  base::StringToDouble(spec->details().total->amount->value, &total);

  base::Value order_info(base::Value::Type::DICTIONARY);
  order_info.SetDoubleKey("total", total);

  base::Value params(base::Value::Type::DICTIONARY);
  params.SetKey("orderInfo", std::move(order_info));

  ShowConstrainedWebDialogWithAutoResize(
      initiator->GetBrowserContext(),
      std::make_unique<CheckoutDialogDelegate>(std::move(params), request),
      initiator, gfx::Size(kDialogMinWidth, kDialogMinHeight),
      gfx::Size(kDialogMaxWidth, kDialogMaxHeight));
}

void ShowCheckoutDialog(WebContents* initiator, base::WeakPtr<PaymentRequest> request) {
  auto* context = initiator->GetBrowserContext();
  Profile* profile = Profile::FromBrowserContext(context);
  if (profile->IsOffTheRecord()) {
    base::PostTask(
      FROM_HERE, {content::BrowserThread::UI},
      base::BindOnce(&PostPaymentRequestErrors,
                     request,
                     PaymentErrorReason::NOT_SUPPORTED,
                     errors::kUnavailableInPrivateMode));
    return;
  }
  auto *service = RewardsServiceFactory::GetForProfile(profile);
  service->StartProcess(base::DoNothing());

  auto* browser = BrowserList::GetInstance()->GetLastActive();
  if (!browser) {
      return;
  }

  auto* web_contents = browser->tab_strip_model()->GetActiveWebContents();

  service->GetPublisherInfo(
      web_contents->GetLastCommittedURL().GetOrigin().host(),
      base::BindOnce(
          &GetPublisherDetailsCallback,
          initiator,
          request
      )
  );
}

}  // namespace brave_rewards
