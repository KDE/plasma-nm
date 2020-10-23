/*
    Copyright 2011 Ilia Kats <ilia-kats@gmx.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "openconnectauthworkerthread.h"

#include <QMutex>
#include <QWaitCondition>
#include <QString>
#include <QByteArray>

extern "C"
{
#include <stdlib.h>
#if !OPENCONNECT_CHECK_VER(1,5)
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/ossl_typ.h>
#endif
#include <errno.h>
}

#include <cstdarg>

class OpenconnectAuthStaticWrapper
{
public:
#if OPENCONNECT_CHECK_VER(5,0)
    static int writeNewConfig(void *obj, const char *str, int num)
    {
        if (obj)
            return static_cast<OpenconnectAuthWorkerThread*>(obj)->writeNewConfig(str, num);
        return -1;
    }
    static int validatePeerCert(void *obj, const char *str)
    {
        if (obj)
            return static_cast<OpenconnectAuthWorkerThread*>(obj)->validatePeerCert(nullptr, str);
        return -1;
    }
#else
    static int writeNewConfig(void *obj, char *str, int num)
    {
        if (obj)
            return static_cast<OpenconnectAuthWorkerThread*>(obj)->writeNewConfig(str, num);
        return -1;
    }
    static int validatePeerCert(void *obj, OPENCONNECT_X509 *cert, const char *str)
    {
        if (obj)
            return static_cast<OpenconnectAuthWorkerThread*>(obj)->validatePeerCert(cert, str);
        return -1;
    }
#endif
	static int processAuthForm(void *obj, struct oc_auth_form *form)
    {
        if (obj)
            return static_cast<OpenconnectAuthWorkerThread*>(obj)->processAuthFormP(form);
        return OC_FORM_RESULT_ERR;
    }
    static void writeProgress(void *obj, int level, const char *str, ...)
    {
        if (obj) {
            va_list argPtr;
            va_start(argPtr, str);
            static_cast<OpenconnectAuthWorkerThread*>(obj)->writeProgress(level, str, argPtr);
            va_end(argPtr);
        }
    }
};

OpenconnectAuthWorkerThread::OpenconnectAuthWorkerThread(QMutex *mutex, QWaitCondition *waitForUserInput, bool *userDecidedToQuit, bool *formGroupChanged, int cancelFd)
	: QThread()
    , m_mutex(mutex)
    , m_waitForUserInput(waitForUserInput)
    , m_userDecidedToQuit(userDecidedToQuit)
    , m_formGroupChanged(formGroupChanged)
{
    m_openconnectInfo = openconnect_vpninfo_new((char*)"OpenConnect VPN Agent (PlasmaNM - running on KDE)",
                                                OpenconnectAuthStaticWrapper::validatePeerCert,
                                                OpenconnectAuthStaticWrapper::writeNewConfig,
                                                OpenconnectAuthStaticWrapper::processAuthForm,
                                                OpenconnectAuthStaticWrapper::writeProgress,
                                                this);
#if OPENCONNECT_CHECK_VER(1,4)
    openconnect_set_cancel_fd(m_openconnectInfo, cancelFd);
#else
    // Silence warning about unused parameter
    Q_UNUSED(cancelFd);
#endif
}

OpenconnectAuthWorkerThread::~OpenconnectAuthWorkerThread()
{
    openconnect_vpninfo_free(m_openconnectInfo);
}

void OpenconnectAuthWorkerThread::run()
{
    openconnect_init_ssl();
    Q_EMIT initTokens();
    int ret = openconnect_obtain_cookie(m_openconnectInfo);
    if (*m_userDecidedToQuit) {
        return;
    }
    Q_EMIT cookieObtained(ret);
}

struct openconnect_info* OpenconnectAuthWorkerThread::getOpenconnectInfo()
{
    return m_openconnectInfo;
}

int OpenconnectAuthWorkerThread::writeNewConfig(const char *buf, int buflen)
{
    Q_UNUSED(buflen)
    if (*m_userDecidedToQuit) {
        return -EINVAL;
    }
    Q_EMIT writeNewConfig(QString(QByteArray(buf).toBase64()));
    return 0;
}

#if !OPENCONNECT_CHECK_VER(1,5)
static char *openconnect_get_cert_details(struct openconnect_info *vpninfo,
                                          OPENCONNECT_X509 *cert)
{
    Q_UNUSED(vpninfo)

    BIO *bp = BIO_new(BIO_s_mem());
    BUF_MEM *certinfo;
    char zero = 0;
    char *ret;

    X509_print_ex(bp, cert, 0, 0);
    BIO_write(bp, &zero, 1);
    BIO_get_mem_ptr(bp, &certinfo);

    ret = strdup(certinfo->data);
    BIO_free(bp);

    return ret;
}
#endif

int OpenconnectAuthWorkerThread::validatePeerCert(void *cert, const char *reason)
{
    if (*m_userDecidedToQuit) {
        return -EINVAL;
    }

#if OPENCONNECT_CHECK_VER(5,0)
    (void)cert;
    const char *fingerprint = openconnect_get_peer_cert_hash(m_openconnectInfo);
    char *details = openconnect_get_peer_cert_details(m_openconnectInfo);
#else
    char fingerprint[41];
    int ret = 0;

    ret = openconnect_get_cert_sha1(m_openconnectInfo, cert, fingerprint);
    if (ret) {
        return ret;
    }

    char *details = openconnect_get_cert_details(m_openconnectInfo, cert);
#endif
    bool accepted = false;
    m_mutex->lock();
    QString qFingerprint(fingerprint);
    QString qCertinfo(details);
    QString qReason(reason);
    Q_EMIT validatePeerCert(qFingerprint, qCertinfo, qReason, &accepted);
    m_waitForUserInput->wait(m_mutex);
    m_mutex->unlock();
    openconnect_free_cert_info(m_openconnectInfo, details);
    if (*m_userDecidedToQuit) {
        return -EINVAL;
    }

    if (accepted) {
        return 0;
    } else {
        return -EINVAL;
    }

}

int OpenconnectAuthWorkerThread::processAuthFormP(struct oc_auth_form *form)
{
    if (*m_userDecidedToQuit) {
        return -1;
    }

    m_mutex->lock();
    *m_formGroupChanged = false;
    Q_EMIT processAuthForm(form);
    m_waitForUserInput->wait(m_mutex);
    m_mutex->unlock();
    if (*m_userDecidedToQuit) {
        return OC_FORM_RESULT_CANCELLED;
    }

    if (*m_formGroupChanged) {
        return OC_FORM_RESULT_NEWGROUP;
    }
    return OC_FORM_RESULT_OK;
}

void OpenconnectAuthWorkerThread::writeProgress(int level, const char *fmt, va_list argPtr)
{
    if (*m_userDecidedToQuit) {
        return;
    }
    const QString msg = QString::vasprintf(fmt, argPtr);
    Q_EMIT updateLog(msg, level);
}
