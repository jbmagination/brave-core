/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

import { DialogCloseReason, OrderInfo, Host, HostListener } from './interfaces'

function parseDialogArgs (): OrderInfo {
  const argString = chrome.getVariableValue('dialogArguments')

  let args: any
  try {
    args = JSON.parse(argString)
  } catch {
    // Ignore
  }

  const { orderInfo } = Object(args)
  // TODO(zenparsing): Throw if orderInfo is invalid?

  console.log("In host.ts!! - Order Info")
  console.log(orderInfo)

  return {
    total: orderInfo.total
  }
}

interface BalanceDetails {
  total: number
}

interface RateDetails {
  rate: number
  lastUpdated: string
}

interface ExternalWalletDetails {
  status: number
}

interface PublisherDetails {
  status: number
  name: string
}

export function createHost (): Host {

  let hostListener: HostListener | null = null
  let balanceDetails: BalanceDetails | null = null
  let rateDetails: RateDetails | null = null
  let externalWalletDetails: ExternalWalletDetails | null = null
  let publisherDetails: PublisherDetails | null = null

  let orderInfo = parseDialogArgs()

  // TODO(zenparsing): Is this required?
  self.i18nTemplate.process(document, self.loadTimeData)

  function sendWalletInfo () {
    console.log("Balance Details")
    console.log(balanceDetails)

    console.log("External Wallet Details")
    console.log(externalWalletDetails)

    if (externalWalletDetails && hostListener) {
      console.log("Sending Wallet Info!!")
      const balance = balanceDetails ? balanceDetails.total : 0
      // TODO(zenparsing): Any other status codes?
      const verified = externalWalletDetails.status === 2
      hostListener.onWalletUpdated({ balance, verified })
    }
  }

  function sendPublisherInfo () {
    if (publisherDetails && hostListener) {
      const name = publisherDetails.name
      const verified = publisherDetails.status === 2
      hostListener.onPublisherUpdated({ verified, name })
    }
  }

  function sendRateInfo() {
    console.log("Sending Rate Info!")
    console.log("Host Listener")
    console.log(hostListener)
    console.log("Rate Details")
    console.log(rateDetails)
    if (hostListener && rateDetails) {
      const rate = rateDetails.rate
      const lastUpdated = new Date(Number(rateDetails.lastUpdated)).toISOString()
      hostListener.onRatesUpdated({rate, lastUpdated})
    }
  }

  self.cr.addWebUIListener('walletBalanceUpdated', (event: any) => {
    if (!event) {
      return
    }

    console.log("Wallet Balance Updated")
    console.log(event)

    balanceDetails = event as BalanceDetails

    console.log("Balance Details")
    console.log(balanceDetails)

    sendWalletInfo()
  })

  self.cr.addWebUIListener('publisherDetailsUpdated', (event: any) => {
    if (!event) {
      return
    }

    publisherDetails = event as PublisherDetails

    console.log("Publisher Details")
    console.log(publisherDetails)

    sendPublisherInfo()
  })

  self.cr.addWebUIListener('rewardsParametersUpdated', (event: any) => {
    console.log("Rewards params updated")
    console.log(event)
    if (!event) {
      return
    }
    rateDetails = event as RateDetails
    console.log("Rate Details")
    console.log(rateDetails)
    sendRateInfo()
  })

  self.cr.addWebUIListener('externalWalletUpdated', (event: any) => {
    console.log("On external wallet updated")
    console.log(event)
    if (!event) {
      return
    }
    externalWalletDetails = event as ExternalWalletDetails
    sendWalletInfo()
  })

  console.log("Getting wallet balance")

  return {
    getLocaleString (key: string) {
      return self.loadTimeData.getString(key)
    },

    closeDialog (reason: DialogCloseReason) {
      chrome.send('dialogClose', [reason.toString()])
    },

    payWithWallet (...args) {
      console.log('payWithWallet', ...args)
      chrome.send('paymentRequestComplete')
      // TODO(zenparsing): Send update to service
    },

    setListener (listener) {
      hostListener = listener

      chrome.send('getWalletBalance')
      chrome.send('getRewardsParameters')
      chrome.send('getWalletBalance')
      chrome.send('getExternalWallet')
      chrome.send('getPublisherDetails')

      queueMicrotask(() => {
        if (hostListener) {
          hostListener.onOrderUpdated(orderInfo)
        }
      })

      return () => { hostListener = null }
    }

  }
}
