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

Client::Client()
{
    m_address=QLatin1String("https://localhost");
    m_port=9779;
    
    clog<<"Client constructor"<<endl;
}

Client::~Client()
{
    clog<<"Client destructor"<<endl;
}

void Client::push(Proxy* proxy, QString plugin, QString method, QVariantList params)
{
    
    
    
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
    /*
    QVariantList value;
    value.append(QLatin1String("True survivor"));
    
    emit response(value);
    
    m_client->touch();
    */
}

void Proxy::push()
{
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