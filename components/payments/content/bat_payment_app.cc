/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/payments/content/bat_payment_app.h"

#include "base/strings/stringprintf.cc"
#include "base/strings/utf_string_conversions.h"
#include "components/payments/core/payer_data.h"

namespace payments {

  BatPaymentApp::BatPaymentApp(base::WeakPtr<PaymentRequestSpec> spec):
      PaymentApp(0, PaymentApp::Type::INTERNAL), 
      spec_(spec) {
      app_method_names_.insert("bat");
  }
  
  BatPaymentApp::~BatPaymentApp() {}

  void BatPaymentApp::InvokePaymentApp(Delegate* delegate) {
    DCHECK(delegate);
    //DCHECK(!delegate_);

    delegate_ = delegate;
    auto transaction_id = spec_->details_ptr()->id.value();
    auto details = base::StringPrintf("{\"order_id\":\"%s\"}", transaction_id.c_str());
    delegate_->OnInstrumentDetailsReady("bat", details, PayerData());
    delegate_ = nullptr;
  }

  bool BatPaymentApp::IsCompleteForPayment() const { return true; }
  uint32_t BatPaymentApp::GetCompletenessScore() const { return 0; }
  bool BatPaymentApp::CanPreselect() const  { return true; }
  base::string16 BatPaymentApp::GetMissingInfoLabel() const  { return base::EmptyString16(); }
  bool BatPaymentApp::HasEnrolledInstrument() const { return false; }
  void BatPaymentApp::RecordUse()  { return; }
  bool BatPaymentApp::NeedsInstallation() const { return false; }

  std::string BatPaymentApp::GetId() const {
    return "bat";
  }
  
  base::string16 BatPaymentApp::GetLabel() const {
    return base::ASCIIToUTF16("bat");
  }
  
  base::string16 BatPaymentApp::GetSublabel() const { return base::EmptyString16(); }
  bool BatPaymentApp::IsValidForModifier(
      const std::string& method,
      bool supported_networks_specified,
      const std::set<std::string>& supported_networks) const { return false; }
  
  base::WeakPtr<PaymentApp> BatPaymentApp::AsWeakPtr() { return weak_ptr_factory_.GetWeakPtr(); }

  bool BatPaymentApp::HandlesShippingAddress() const { return false; }
  bool BatPaymentApp::HandlesPayerName() const { return false; }
  bool BatPaymentApp::HandlesPayerEmail() const { return false; }
  bool BatPaymentApp::HandlesPayerPhone() const { return false; }

} //namespace payments