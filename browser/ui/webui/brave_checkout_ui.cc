/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_checkout_ui.h"

#include <memory>
#include <utility>

#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "brave/browser/ui/webui/brave_webui_source.h"
#include "brave/browser/brave_rewards/rewards_service_factory.h"
#include "brave/components/brave_rewards/browser/rewards_service.h"
#include "brave/components/brave_rewards/browser/rewards_service_observer.h"
#include "brave/components/brave_rewards/resources/grit/brave_rewards_checkout_generated_map.h"
#include "brave/components/brave_rewards/resources/grit/brave_rewards_resources.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser.h"
#include "chrome/browser/ui/browser_list.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui_data_source.h"
#include "content/public/browser/web_ui_message_handler.h"

namespace {

using brave_rewards::RewardsService;
using brave_rewards::RewardsServiceFactory;
using brave_rewards::RewardsServiceObserver;

class CheckoutMessageHandler : public content::WebUIMessageHandler {
 public:
  CheckoutMessageHandler();
  ~CheckoutMessageHandler() override;

  void RegisterMessages() override;

 private:
  RewardsService* GetRewardsService();

  // Message handlers
  void OnGetWalletBalance(const base::ListValue* args);
  void OnGetPublisherDetails(const base::ListValue* args);
  void GetExternalWallet(const base::ListValue* args);
  void GetRewardsParameters(const base::ListValue* args);

  // Rewards service callbacks
  void FetchBalanceCallback(const ledger::type::Result result,
                            ledger::type::BalancePtr balance);

  void GetExternalWalletCallback(
      const ledger::type::Result result,
      ledger::type::ExternalWalletPtr wallet);
  void GetRewardsParametersCallback(
      ledger::type::RewardsParametersPtr parameters);
  void GetPublisherDetailsCallback(
      const ledger::type::Result result,
      ledger::type::PublisherInfoPtr info);

  RewardsService* rewards_service_ = nullptr;  // NOT OWNED
  base::WeakPtrFactory<CheckoutMessageHandler> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(CheckoutMessageHandler);
};

CheckoutMessageHandler::CheckoutMessageHandler() : weak_factory_(this) {}

CheckoutMessageHandler::~CheckoutMessageHandler() = default;

RewardsService* CheckoutMessageHandler::GetRewardsService() {
  if (!rewards_service_) {
    Profile* profile = Profile::FromWebUI(web_ui());
    rewards_service_ = RewardsServiceFactory::GetForProfile(profile);
    rewards_service_->StartProcess(base::DoNothing());
  }
  return rewards_service_;
}

void CheckoutMessageHandler::RegisterMessages() {
  web_ui()->RegisterMessageCallback(
      "getWalletBalance",
      base::BindRepeating(&CheckoutMessageHandler::OnGetWalletBalance,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "getExternalWallet",
      base::BindRepeating(&CheckoutMessageHandler::GetExternalWallet,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "getRewardsParameters",
      base::BindRepeating(&CheckoutMessageHandler::GetRewardsParameters,
                          base::Unretained(this)));

  web_ui()->RegisterMessageCallback(
      "getPublisherDetails",
      base::BindRepeating(&CheckoutMessageHandler::OnGetPublisherDetails,
                          base::Unretained(this)));
}

void CheckoutMessageHandler::OnGetPublisherDetails(const base::ListValue* args) {
  if (auto* service = GetRewardsService()) {
    auto* browser = BrowserList::GetInstance()->GetLastActive();
    if (!browser) {
      return;
    }

    auto* web_contents = browser->tab_strip_model()->GetActiveWebContents();

    service->GetPublisherInfo(
      web_contents->GetLastCommittedURL().GetOrigin().host(),
      base::BindOnce(
          &CheckoutMessageHandler::GetPublisherDetailsCallback,
          weak_factory_.GetWeakPtr()));
  }
}

void CheckoutMessageHandler::GetPublisherDetailsCallback(
    const ledger::type::Result result,
    ledger::type::PublisherInfoPtr info) {
  if (!info) {
    return;
  }

  base::Value dict(base::Value::Type::DICTIONARY);
  dict.SetIntKey("status", static_cast<int>(info->status));
  dict.SetStringKey("name", info->name);

  FireWebUIListener("publisherDetailsUpdated", dict);
}

void CheckoutMessageHandler::GetRewardsParameters(const base::ListValue* args) {
  if (auto* service = GetRewardsService()) {
    AllowJavascript();
    service->GetRewardsParameters(base::Bind(
        &CheckoutMessageHandler::GetRewardsParametersCallback,
        weak_factory_.GetWeakPtr()));
  }
}

void CheckoutMessageHandler::GetRewardsParametersCallback(
    ledger::type::RewardsParametersPtr parameters) {
  if (!IsJavascriptAllowed()) {
    return;
  }

  base::Value data(base::Value::Type::DICTIONARY);
  if (parameters) {
    data.SetDoubleKey("rate", parameters->rate);
    data.SetDoubleKey("lastUpdated", 
                      base::Time::Now().ToJsTimeIgnoringNull());
  }
  FireWebUIListener("rewardsParametersUpdated", data);
}

void CheckoutMessageHandler::OnGetWalletBalance(const base::ListValue* args) {
  if (auto* service = GetRewardsService()) {
    AllowJavascript();
    service->FetchBalance(
        base::BindOnce(&CheckoutMessageHandler::FetchBalanceCallback,
                       weak_factory_.GetWeakPtr()));
  }
}

void CheckoutMessageHandler::FetchBalanceCallback(
    const ledger::type::Result result,
    ledger::type::BalancePtr balance) {
  if (!IsJavascriptAllowed()) {
    return;
  }

  base::Value data(base::Value::Type::DICTIONARY);
  data.SetDoubleKey("total", balance->total);

  FireWebUIListener("walletBalanceUpdated", data);
}

void CheckoutMessageHandler::GetExternalWallet(const base::ListValue* args) {
  if (auto* service = GetRewardsService()) {
    AllowJavascript();
    service->GetExternalWallet(
        base::BindOnce(&CheckoutMessageHandler::GetExternalWalletCallback,
                       weak_factory_.GetWeakPtr()));
  }
}

void CheckoutMessageHandler::GetExternalWalletCallback(
    const ledger::type::Result result,
    ledger::type::ExternalWalletPtr wallet) {
  if (!IsJavascriptAllowed()) {
    return;
  }

  base::Value data(base::Value::Type::DICTIONARY);

  if (wallet) {
    data.SetIntKey("status", static_cast<int>(wallet->status));
  }
  FireWebUIListener("externalWalletUpdated", data);
}

}  // namespace

BraveCheckoutUI::BraveCheckoutUI(content::WebUI* web_ui,
                                 const std::string& name)
    : ConstrainedWebDialogUI(web_ui) {
  Profile* profile = Profile::FromWebUI(web_ui);
  DCHECK(!profile->IsOffTheRecord());
  CreateAndAddWebUIDataSource(web_ui, name, kBraveRewardsCheckoutGenerated,
                              kBraveRewardsCheckoutGeneratedSize,
                              IDR_BRAVE_REWARDS_CHECKOUT_HTML);

  web_ui->AddMessageHandler(std::make_unique<CheckoutMessageHandler>());
}

BraveCheckoutUI::~BraveCheckoutUI() {}
