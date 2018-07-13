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

#ifndef OPENCONNECTAUTHWORKERTHREAD_H
#define OPENCONNECTAUTHWORKERTHREAD_H

extern "C" {
#include <openconnect.h>
}

#if OPENCONNECT_API_VERSION_MAJOR == 1
#define openconnect_vpninfo_new openconnect_vpninfo_new_with_cbdata
#define openconnect_init_ssl openconnect_init_openssl
#endif

#ifndef OPENCONNECT_CHECK_VER
#define OPENCONNECT_CHECK_VER(x,y) 0
#endif

#if !OPENCONNECT_CHECK_VER(1,5)
struct x509_st;
#define OPENCONNECT_X509 struct x509_st
#define OPENCONNECT_OPENSSL
#endif

#if OPENCONNECT_CHECK_VER(3,0)
#define NEWGROUP_SUPPORTED	1
#define AUTHGROUP_OPT(form)	(void *)(form)->authgroup_opt
#define AUTHGROUP_SELECTION(form) (form)->authgroup_selection
#define FORMCHOICE(sopt, i)	((sopt)->choices[i])
#define IGNORE_OPT(opt)		((opt)->flags & OC_FORM_OPT_IGNORE)
#else
#define NEWGROUP_SUPPORTED	0
#define AUTHGROUP_OPT(form)	nullptr
#define AUTHGROUP_SELECTION(form) 0
#define FORMCHOICE(sopt, i)	(&(sopt)->choices[i])
#define IGNORE_OPT(opt)		0

#define OC_FORM_RESULT_ERR	-1
#define OC_FORM_RESULT_OK	0
#define OC_FORM_RESULT_CANCELLED 1
#define OC_FORM_RESULT_NEWGROUP	2
#endif

#if OPENCONNECT_CHECK_VER(4,0)
#define OC3DUP(x)			(x)
#else
#define openconnect_set_option_value(opt, val) do { \
		struct oc_form_opt *_o = (opt);				\
		free(_o->value); _o->value = strdup(val);		\
	} while (0)
#define openconnect_free_cert_info(v, x) ::free(x)
#define OC3DUP(x)			strdup(x)
#endif

#include <QThread>

class QMutex;
class QWaitCondition;
struct openconnect_info;

class OpenconnectAuthWorkerThread : public QThread
{
    Q_OBJECT
    friend class OpenconnectAuthStaticWrapper;
public:
    OpenconnectAuthWorkerThread(QMutex *, QWaitCondition *, bool *, bool *, int);
    ~OpenconnectAuthWorkerThread() override;
    struct openconnect_info* getOpenconnectInfo();

Q_SIGNALS:
    void validatePeerCert(const QString &, const QString &, const QString &, bool*);
    void processAuthForm(struct oc_auth_form *);
    void updateLog(const QString &, const int&);
    void writeNewConfig(const QString &);
    void cookieObtained(const int&);

protected:
    void run() override;

private:
    int writeNewConfig(const char *, int);
    int validatePeerCert(void *, const char *);
    int processAuthFormP(struct oc_auth_form *);
    void writeProgress(int level, const char *, va_list);

    QMutex *m_mutex;
    QWaitCondition *m_waitForUserInput;
    bool *m_userDecidedToQuit;
    bool *m_formGroupChanged;
    struct openconnect_info *m_openconnectInfo;
};

#endif
