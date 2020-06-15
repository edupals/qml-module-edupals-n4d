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

#ifndef QML_EDUPALS_N4D_PLUGIN
#define QML_EDUPALS_N4D_PLUGIN

#include <n4d.hpp>

#include <QQmlExtensionPlugin>
#include <QObject>
#include <QVariant>
#include <QString>
#include <QList>
#include <QThread>

class Proxy;

class Worker: public QThread
{
    Q_OBJECT
    
protected:

    QString m_address;
    int m_port;

    QString m_user;
    QString m_password;

public:
    Worker(QString user,QString password,QString address,int port) :
        m_user(user),
        m_password(password),
        m_address(address),
        m_port(port) {
            
        }
    
public Q_SLOTS:
    void push(Proxy* proxy, QString plugin, QString method, QVariantList params);

Q_SIGNALS:
    void result(Proxy* proxy,QVariantList value);
};

class Client: public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString address MEMBER m_address)
    Q_PROPERTY(int port MEMBER m_port)
    Q_PROPERTY(QString user MEMBER m_user)
    Q_PROPERTY(QString password MEMBER m_password)

protected:
    
    QString m_address;
    int m_port;
    
    QString m_user;
    QString m_password;
    
    Worker* m_worker;
    
public:
    
    Client();
    ~Client();

protected Q_SLOTS:
    void onResult(Proxy* proxy, QVariantList value);
    
public Q_SLOTS:
    void push(Proxy* proxy, QString plugin, QString method, QVariantList params);
};

class Proxy: public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString plugin MEMBER m_plugin)
    Q_PROPERTY(QString method MEMBER m_method)
    Q_PROPERTY(Client* client MEMBER m_client)
    
protected:
    QString m_plugin;
    QString m_method;
    Client* m_client;
    
public:
    Proxy();
    Q_INVOKABLE void call(QVariantList params);
    
public Q_SLOTS:
    void push(QVariantList value);

Q_SIGNALS:
    void response(QVariantList value);
    void error(QString what);

};

class N4DPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA (IID "Edupals.N4D")

public:
    explicit N4DPlugin(QObject *parent = nullptr);
    void registerTypes(const char *uri) override;
};
 

#endif