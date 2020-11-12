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
    
    n4d::Client nc;
    
    if (job->m_user.size()!=0) {
        nc=n4d::Client (job->m_address.toStdString(),
               job->m_port,
               job->m_user.toStdString(),
               job->m_password.toStdString());
    }
    else {
        nc=n4d::Client (job->m_address.toStdString(),
           job->m_port);
    }
    
    try {
        res = nc.call(job->m_plugin.toStdString(),
                      job->m_method.toStdString());
        
        QVariant value = convert(res);
        emit result(job,value);
    }
    catch (std::exception& e) {
        emit error(job,1,QString::fromUtf8(e.what()));
    }
}

Client::Client()
{
    m_address=QLatin1String("https://localhost");
    m_port=9800;
    m_credentialType = Client::Anonymous;
    
    m_worker = new Worker();
    
    connect(m_worker,&Worker::result,this,&Client::onResult);
    connect(m_worker,&Worker::error,this,&Client::onError);
    
}

Client::~Client()
{
    delete m_worker;
}

void Client::onResult(Job* job, QVariant value)
{
    job->m_proxy->push(value);
    
    delete job;
}

void Client::onError(Job* job,int code, QString what)
{
    job->m_proxy->push(code,what);
    delete job;
}

void Client::push(Proxy* proxy, QString plugin, QString method, QVariantList params)
{
    Job* job;

    job = new Job(proxy,m_address,m_port,m_user,m_password,m_key,m_credential,plugin,method,params);
    
    QMetaObject::invokeMethod(m_worker,"push",Qt::QueuedConnection,
            Q_ARG(Job*,job));
}

Proxy::Proxy()
{
    m_client=nullptr;
}

void Proxy::call(QVariantList params)
{
    if (m_client) {
        m_client->push(this,m_plugin,m_method,params);
    }
}

void Proxy::push(QVariant value)
{
    emit response(value);
}

void Proxy::push(int code,QString what)
{
    emit error(code,what);
}

N4DPlugin::N4DPlugin(QObject* parent) : QQmlExtensionPlugin(parent)
{
}

void N4DPlugin::registerTypes(const char* uri)
{
    qmlRegisterType<Client> (uri, 1, 0, "Client");
    qmlRegisterType<Proxy> (uri, 1, 0, "Proxy");
    qmlRegisterAnonymousType<QMimeData>(uri, 1);
    
}
