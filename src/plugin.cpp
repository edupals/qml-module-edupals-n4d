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

using namespace edupals;
using namespace std;

void Worker::push(Job* job)
{
    for (int n=0;n<job->m_params.count();n++) {
        clog<<"type: "<<job->m_params[n].typeName()<<endl;
        variant::Variant v = convert(job->m_params[n]);
        clog<<"variant:"<<v<<endl;
    }
    
    QVariantList value;
    QString v = QLatin1String("call::")+job->m_plugin+QLatin1String("::")+job->m_method;
    value.append(v);
    
    emit result(job,value);
}

Client::Client()
{
    m_address=QLatin1String("https://localhost");
    m_port=9779;
    
    clog<<"Client constructor"<<endl;
    
    m_worker = new Worker();
    
    connect(m_worker,&Worker::result,this,&Client::onResult);
}

Client::~Client()
{
    clog<<"Client destructor"<<endl;
    
    m_worker->quit();
    m_worker->wait(1000);
    delete m_worker;
}

void Client::onResult(Job* job, QVariantList value)
{
    clog<<"result:"<<endl;
    for (int n=0;n<value.count();n++) {
        clog<<"* "<<value[n].toString().toStdString()<<endl;
    }
    
    job->m_proxy->push(value);
    
    delete job;
}

void Client::push(Proxy* proxy, QString plugin, QString method, QVariantList params)
{
    Job* job = new Job(proxy,m_address,m_port,m_user,m_password,plugin,method,params);
    
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

void Proxy::push(QVariantList value)
{
    emit response(value);
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