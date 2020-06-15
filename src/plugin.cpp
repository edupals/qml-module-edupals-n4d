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

#include <QQmlEngine>
#include <QAbstractItemModel>
#include <QMimeData>

#include <iostream>

using namespace std;

void Worker::push(Proxy* proxy, QString plugin, QString method, QVariantList params)
{
    QVariantList value;
    QString v = QLatin1String("call::")+plugin+QLatin1String("::")+method;
    value.append(v);
    
    emit result(proxy,value);
}

Client::Client()
{
    m_address=QLatin1String("https://localhost");
    m_port=9779;
    
    clog<<"Client constructor"<<endl;
    
    m_worker = new Worker(m_user,m_password,m_address,m_port);
    
    connect(m_worker,&Worker::result,this,&Client::onResult);
}

Client::~Client()
{
    clog<<"Client destructor"<<endl;
    
    m_worker->quit();
    m_worker->wait(1000);
    delete m_worker;
}

void Client::onResult(Proxy* proxy, QVariantList value)
{
    clog<<"result:"<<endl;
    for (int n=0;n<value.count();n++) {
        clog<<"* "<<value[n].toString().toStdString()<<endl;
    }
    
    proxy->push(value);
}

void Client::push(Proxy* proxy, QString plugin, QString method, QVariantList params)
{
    QMetaObject::invokeMethod(m_worker,"push",Qt::QueuedConnection,
            Q_ARG(Proxy*,proxy),
            Q_ARG(QString,plugin),
            Q_ARG(QString,method),
            Q_ARG(QVariantList,params)
    );
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