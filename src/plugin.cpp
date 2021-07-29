/*
 * Copyright (C) 2020 Edupals project
 *
 * Author:
 *  Enrique Medina Gremaldos <quiqueiii@gmail.com>
 *
 * Source:
 *  https://github.com/edupals/qml-module-edupals-n4d
 *
 * This file is a part of qml-module-edupals-n4d.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#include "plugin.h"
#include "variant.h"

#include <n4d.hpp>
#include <variant.hpp>

#include <QQmlEngine>
#include <QAbstractItemModel>
#include <QMimeData>

#include <iostream>
#include <vector>

using namespace edupals;
using namespace std;

Worker::Worker()
{
    m_thread = new QThread();
    moveToThread(m_thread);
    
    m_thread->start();
}

Worker::~Worker()
{
    m_thread->quit();
    m_thread->wait(1000);
    
    delete m_thread;
}

void Worker::push(Job* job)
{
    vector<variant::Variant> params;
    variant::Variant res;
    
    for (int n=0;n<job->m_params.count();n++) {
        variant::Variant v = convert(job->m_params[n]);
        
        params.push_back(v);
    }
    
    n4d::auth::Credential n4dCredential;
    n4d::auth::Key key;
    
    switch (job->m_credential) {
        case Client::Password:
            n4dCredential = n4d::auth::Credential(job->m_user.toStdString(),job->m_password.toStdString());
        break;
        
        case Client::Key:
            key = n4d::auth::Key(job->m_key.toStdString());
            n4dCredential = n4d::auth::Credential(job->m_user.toStdString(),key);
        break;
        
        case Client::MasterKey:
            
            if (job->m_key.size()!=0) {
                key = n4d::auth::Key(job->m_key.toStdString());
            }
            else {
                // try to load a master key
                key = n4d::auth::Key::master_key();
            }
            
            n4dCredential = n4d::auth::Credential(key);
        break;
        
        case Client::LocalKey:
            key = n4d::auth::Key::user_key(job->m_user.toStdString());
            
            if (!key.valid()) {
                //perform a second attempt
                n4d::Client lc("https://localhost:9779",job->m_user.toStdString(),"");
                
                n4d::Ticket ticket = lc.create_ticket();
                
                if (ticket.valid()) {
                    n4dCredential = ticket.get_credential();
                }
                else {
                    // key is invalid but we push it anyway so user gets a 
                    // consistent error response
                    n4dCredential = n4d::auth::Credential(job->m_user.toStdString(),key);
                }
            }
            else {
                n4dCredential = n4d::auth::Credential(job->m_user.toStdString(),key);
            }
            
        break;
            
        default:
            break;
    }
    
    n4d::Client nc(job->m_address.toStdString(),n4dCredential);
    
    try {
        if (job->m_plugin.size()>0) {
            res = nc.call(job->m_plugin.toStdString(),
                      job->m_method.toStdString(),params);
        }
        else {
            res = nc.builtin_call(job->m_method.toStdString(),params);
        }
        
        QVariant value = convert(res);
        emit result(job,value);
    }
    catch (n4d::exception::UnknownClass& e) {
        emit error(job,Error::UnknownClass,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::UnknownMethod& e) {
        emit error(job,Error::UnknownMethod,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::UserNotAllowed& e) {
        emit error(job,Error::UserNotAllowed,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::AuthenticationFailed& e) {
        emit error(job,Error::AuthenticationFailed,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::InvalidMethodResponse& e) {
        emit error(job,Error::InvalidResponse,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::InvalidServerResponse& e) {
        emit error(job,Error::InvalidServerResponse,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::InvalidArguments& e) {
        emit error(job,Error::InvalidArguments,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::UnhandledError& e) {
        emit error(job,Error::UnhandledError,QString::fromUtf8(e.what()));
    }
    catch (n4d::exception::CallFailed& e) {
        QVariantMap details;
        details[QLatin1String("code")]=e.code;
        details[QLatin1String("message")]=QString::fromStdString(e.message);
        emit error(job,Error::CallFailed,QString::fromUtf8(e.what()),details);
    }
    catch (n4d::exception::UnknownCode& e) {
        emit error(job,Error::UnknownCode,QString::fromUtf8(e.what()));
    }
    catch (std::exception& e) {
        emit error(job,-1000,QString::fromUtf8(e.what()));
    }
}

Client::Client()
{
    m_address=QLatin1String("https://localhost:9779");
    m_credential = Client::Anonymous;
    
    m_worker = new Worker();
    
    connect(m_worker,&Worker::result,this,&Client::onResult);
    connect(m_worker,&Worker::error,this,&Client::onError);
    
}

Client::~Client()
{
    delete m_worker;
}

QVariant Client::call(QString plugin,QString method,QVariantList params)
{
    QVariant ret;
    Worker* worker = new Worker();
    Job* job;
    
    connect(worker,&Worker::result,[&ret] (Job* job, QVariant value) mutable {
        ret.setValue(value);
    });
    connect(worker,&Worker::error,[=] (Job* job,int code, QString what,QVariantMap details) mutable {
        QString msg = QString(QStringLiteral("[%1]:%2")).arg(code).arg(what);
        qmlEngine(this)->throwError(msg);
    });

    job = new Job(nullptr,m_address,m_user,m_password,m_key,m_credential,plugin,method,params);
    
    QMetaObject::invokeMethod(worker,"push",Qt::BlockingQueuedConnection,
            Q_ARG(Job*,job));
    
    delete worker;
    
    return ret;
}

void Client::onResult(Job* job, QVariant value)
{
    job->m_proxy->push(value);
    
    delete job;
}

void Client::onError(Job* job,int code, QString what,QVariantMap details)
{
    job->m_proxy->push(code,what,details);
    delete job;
}

void Client::push(Proxy* proxy, QString plugin, QString method, QVariantList params)
{
    Job* job;

    job = new Job(proxy,m_address,m_user,m_password,m_key,m_credential,plugin,method,params);
    
    QMetaObject::invokeMethod(m_worker,"push",Qt::QueuedConnection,
            Q_ARG(Job*,job));
}

Proxy::Proxy() : m_busy(false), m_client(nullptr)
{
    emit busyChanged();
}

void Proxy::call(QVariantList params)
{
    if (m_client) {
        m_busy=true;
        emit busyChanged();
        
        m_client->push(this,m_plugin,m_method,params);
    }
}

void Proxy::push(QVariant value)
{
    m_busy=false;
    emit busyChanged();
    emit response(value);
}

void Proxy::push(int code,QString what,QVariantMap details)
{
    m_busy=false;
    emit busyChanged();
    emit error(code,what,details);
}

N4DPlugin::N4DPlugin(QObject* parent) : QQmlExtensionPlugin(parent)
{
}

void N4DPlugin::registerTypes(const char* uri)
{
    qmlRegisterType<Error> (uri, 1, 0, "Error");
    qmlRegisterType<Client> (uri, 1, 0, "Client");
    qmlRegisterType<Proxy> (uri, 1, 0, "Proxy");
    qmlRegisterAnonymousType<QMimeData>(uri, 1);
    
}
