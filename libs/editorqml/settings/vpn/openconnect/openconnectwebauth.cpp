/*
    SPDX-FileCopyrightText: 2026 Tushar Gupta <tushar.197712@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "openconnectwebauth.h"

#include <QWebEngineWebAuthUxRequest>

#include <KLocalizedString>

OpenconnectWebAuth::OpenconnectWebAuth(QObject *parent)
    : QObject(parent)
{
}

OpenconnectWebAuth::~OpenconnectWebAuth() = default;

void OpenconnectWebAuth::setRequest(QWebEngineWebAuthUxRequest *request)
{
    if (m_request == request) {
        return;
    }

    if (m_request) {
        disconnect(m_request, nullptr, this, nullptr);
    }

    m_request = request;

    if (m_request) {
        connect(m_request, &QWebEngineWebAuthUxRequest::stateChanged, this, &OpenconnectWebAuth::onStateChanged);
    }

    Q_EMIT changed();
    Q_EMIT activeChanged(state() != Inactive);
}

void OpenconnectWebAuth::onStateChanged()
{
    Q_EMIT changed();
    Q_EMIT activeChanged(state() != Inactive);
}

OpenconnectWebAuth::State OpenconnectWebAuth::state() const
{
    if (!m_request) {
        return Inactive;
    }

    switch (m_request->state()) {
    case QWebEngineWebAuthUxRequest::WebAuthUxState::SelectAccount:
        return SelectAccount;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::CollectPin:
        return CollectPin;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::FinishTokenCollection:
        return FinishTokenCollection;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::RequestFailed:
        return RequestFailed;
    default:
        return Inactive;
    }
}

QString OpenconnectWebAuth::heading() const
{
    if (!m_request) {
        return QString();
    }

    switch (state()) {
    case SelectAccount:
        return i18n("Choose a Passkey");
    case FinishTokenCollection:
        return i18n("Use your security key with %1", m_request->relyingPartyId());
    case CollectPin:
        switch (m_request->pinRequest().reason) {
        case QWebEngineWebAuthUxRequest::PinEntryReason::Challenge:
            return i18n("PIN Required");
        case QWebEngineWebAuthUxRequest::PinEntryReason::Set:
            return i18n("New PIN Required");
        default:
            return i18n("Change PIN Required");
        }
    case RequestFailed:
        return i18n("Something went wrong");
    case Inactive:
        break;
    }

    return QString();
}

QString OpenconnectWebAuth::description() const
{
    if (!m_request) {
        return QString();
    }

    switch (state()) {
    case SelectAccount:
        return i18n("Which passkey do you want to use for %1?", m_request->relyingPartyId());
    case FinishTokenCollection:
        return i18n("Touch your security key again to complete the request.");
    case CollectPin:
        switch (m_request->pinRequest().reason) {
        case QWebEngineWebAuthUxRequest::PinEntryReason::Challenge:
            return i18n("Enter the PIN for your security key");
        case QWebEngineWebAuthUxRequest::PinEntryReason::Set:
            return i18n("Set new PIN for your security key");
        default:
            return i18n("Change PIN for your security key");
        }
    case RequestFailed:
        break;
    case Inactive:
        return QString();
    }

    switch (m_request->requestFailureReason()) {
    case QWebEngineWebAuthUxRequest::RequestFailureReason::Timeout:
        return i18n("Request Timeout");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyNotRegistered:
        return i18n("Key not registered");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyAlreadyRegistered:
        return i18n("You already registered this device. Try again with device");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::SoftPinBlock:
        return i18n("The security key is locked because the wrong PIN was entered too many times. To unlock it, remove and reinsert it.");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::HardPinBlock:
        return i18n("The security key is locked because the wrong PIN was entered too many times. You'll need to reset the security key.");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorRemovedDuringPinEntry:
        return i18n("Authenticator removed during verification. Please reinsert and try again");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingResidentKeys:
        return i18n("Authenticator doesn't have resident key support");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingUserVerification:
        return i18n("Authenticator missing user verification");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingLargeBlob:
        return i18n("Authenticator missing Large Blob support");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::NoCommonAlgorithms:
        return i18n("The security token does not support the server’s required authentication method.");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::StorageFull:
        return i18n("Storage Full");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::UserConsentDenied:
        return i18n("User consent denied");
    case QWebEngineWebAuthUxRequest::RequestFailureReason::WinUserCancelled:
        return i18n("User Cancelled Request");
    }

    return QString();
}

QStringList OpenconnectWebAuth::userNames() const
{
    return m_request ? m_request->userNames() : QStringList();
}

bool OpenconnectWebAuth::confirmPinRequired() const
{
    return m_request && state() == CollectPin && m_request->pinRequest().reason != QWebEngineWebAuthUxRequest::PinEntryReason::Challenge;
}

QString OpenconnectWebAuth::pinError() const
{
    if (!m_request || state() != CollectPin) {
        return QString();
    }

    const QWebEngineWebAuthPinRequest pinRequest = m_request->pinRequest();
    const int remaining = pinRequest.remainingAttempts;

    switch (pinRequest.error) {
    case QWebEngineWebAuthUxRequest::PinEntryError::NoError:
        return QString();
    case QWebEngineWebAuthUxRequest::PinEntryError::InternalUvLocked:
        return i18n("Internal User Verification Locked");
    case QWebEngineWebAuthUxRequest::PinEntryError::WrongPin:
        return i18np("Wrong PIN. %1 attempt remaining.", "Wrong PIN. %1 attempts remaining.", remaining);
    case QWebEngineWebAuthUxRequest::PinEntryError::TooShort:
        return i18np("Too Short. %1 attempt remaining.", "Too Short. %1 attempts remaining.", remaining);
    case QWebEngineWebAuthUxRequest::PinEntryError::InvalidCharacters:
        return i18np("Invalid Characters. %1 attempt remaining.", "Invalid Characters. %1 attempts remaining.", remaining);
    case QWebEngineWebAuthUxRequest::PinEntryError::SameAsCurrentPin:
        return i18np("Same as current PIN. %1 attempt remaining.", "Same as current PIN. %1 attempts remaining.", remaining);
    }

    return QString();
}

bool OpenconnectWebAuth::canAccept() const
{
    return state() == SelectAccount || state() == CollectPin;
}

QString OpenconnectWebAuth::acceptText() const
{
    return state() == CollectPin ? i18nc("@action:button submit the entered PIN", "Next") : i18nc("@action:button", "OK");
}

bool OpenconnectWebAuth::canRetry() const
{
    if (!m_request || state() != RequestFailed) {
        return false;
    }

    switch (m_request->requestFailureReason()) {
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyAlreadyRegistered:
    case QWebEngineWebAuthUxRequest::RequestFailureReason::SoftPinBlock:
        return true;
    default:
        return false;
    }
}

QString OpenconnectWebAuth::cancelText() const
{
    return state() == RequestFailed ? i18nc("@action:button", "Close") : i18nc("@action:button", "Cancel");
}

void OpenconnectWebAuth::acceptAccount(const QString &userName)
{
    if (m_request && state() == SelectAccount && !userName.isEmpty()) {
        m_request->setSelectedAccount(userName);
    }
}

void OpenconnectWebAuth::acceptPin(const QString &pin)
{
    if (m_request && state() == CollectPin) {
        m_request->setPin(pin);
    }
}

void OpenconnectWebAuth::cancel()
{
    if (m_request) {
        m_request->cancel();
    }
}

void OpenconnectWebAuth::retry()
{
    if (m_request) {
        m_request->retry();
    }
}

#include "moc_openconnectwebauth.cpp"
