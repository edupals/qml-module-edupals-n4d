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
#include <QVariant>
#include <QJSValue>

class Worker;
class Job;
class Proxy;

class Error: public QObject
{
    Q_OBJECT
    
    public:
        
    enum ErrorCode
        {
            UnknownClass = -40,
            UnknownMethod = -30,
            UserNotAllowed = -20,
            AuthenticationFailed = -10,
            InvalidResponse = -5,
            InvalidArguments = -3,
            UnhandledError = -2,
            CallFailed = -1,
            CallSuccessful = 0,
            InvalidServerResponse = -1001,
            UnknownCode = -1002,
            VariableNotFound = -2001,
            VariableProtected = -2002
        };
        
    Q_ENUM(ErrorCode)
    
};

class Client: public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString address MEMBER m_address)
    Q_PROPERTY(QString user MEMBER m_user)
    Q_PROPERTY(QString password MEMBER m_password)
    Q_PROPERTY(QString key MEMBER m_key)
    Q_PROPERTY(CredentialType credential MEMBER m_credential)
    
public:
    
    enum CredentialType { Anonymous, Password, Key, MasterKey, LocalKey};
    Q_ENUM(CredentialType)
    
    Client();
    ~Client();
    
    /*!
      This is a synchronous (blocking) call. Use it only for development/testing
      purposes
     */
    Q_INVOKABLE QVariant call(QString plugin,QString method,QVariantList params, bool handleVariable = false);

    /*!
        Synchronous call to built-in method get_variable
        @param variableName Variable name to get
     */
    Q_INVOKABLE QVariant getVariable(QString variableName);

    /*!
        Synchronous call to built-in method get_variables
    */
    Q_INVOKABLE QVariant getVariables();

protected:
    
    QString m_address;
    
    QString m_user;
    QString m_password;
    QString m_key;
    
    CredentialType m_credential;
    
    Worker* m_worker;
    
protected Q_SLOTS:
    void onResult(Job* job, QVariant value);
    void onError(Job* job,int code, QString what,QVariantMap details);
    
public Q_SLOTS:
    void push(Proxy* proxy, QString plugin, QString method, QVariantList params);
};

class Proxy: public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString plugin MEMBER m_plugin)
    Q_PROPERTY(QString method MEMBER m_method)
    Q_PROPERTY(Client* client MEMBER m_client)
    Q_PROPERTY(bool busy MEMBER m_busy READ busy NOTIFY busyChanged)
    
protected:
    QString m_plugin;
    QString m_method;
    bool m_busy;
    Client* m_client;
    
public:
    Proxy();
    Q_INVOKABLE void call(QVariantList params);

    bool busy() const
    {
        return m_busy;
    }
    
public Q_SLOTS:
    void push(QVariant value, QVariantList params);
    void push(int code,QString what,QVariantMap details, QVariantList params);

Q_SIGNALS:
    void response(QVariant value, QVariantList params);
    void error(int code,QString what,QVariantMap details, QVariantList params);
    void busyChanged();
};

class Job: public QObject
{
    Q_OBJECT
    
public:
    Proxy* m_proxy;
    
    QString m_address;

    QString m_user;
    QString m_password;
    QString m_key;
    Client::CredentialType m_credential;
    
    QString m_plugin;
    QString m_method;
    
    QVariantList m_params;
    
    Job(Proxy* proxy,QString address,QString user,QString password,QString key, Client::CredentialType credential, QString plugin, QString method, QVariantList params) :
        m_proxy(proxy),
        m_address(address),
        m_user(user),
        m_password(password),
        m_key(key),
        m_credential(credential),
        m_plugin(plugin),
        m_method(method),
        m_params(params) {
            
        }
};

class Worker: public QObject
{
    Q_OBJECT

protected:
    QThread* m_thread;
    
public:
    Worker();
    ~Worker();
    
public Q_SLOTS:
    void push(Job* job);

Q_SIGNALS:
    void result(Job* job,QVariant value);
    void error(Job* job,int code, QString what,QVariantMap details={});
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
