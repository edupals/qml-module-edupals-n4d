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

#include "variant.h"
#include <QMap>
#include <string>

using namespace edupals;
using namespace std;

variant::Variant convert(QVariant in)
{
    variant::Variant ret;
    
    string tname = in.typeName();
    
    if (tname=="int") {
        ret=in.toInt();
    }
    
    if (tname=="double") {
        ret=in.toDouble();
    }
    
    if (tname=="QString") {
        ret=in.toString().toStdString();
    }
    
    if (tname=="QVariantMap") {
        ret = variant::Variant::create_struct();
        QVariantMap m = in.toMap();
        
        QMap<QString, QVariant>::const_iterator i = m.constBegin();
        while (i != m.constEnd()) {
            ret[i.key().toStdString()] = convert(i.value());
            ++i;
        }
    }
    
    if (tname=="QVariantList") {
        ret = variant::Variant::create_array(0);
        QVariantList l = in.toList();
        
        QList<QVariant>::const_iterator i = l.constBegin();
        while (i != l.constEnd()) {
            ret.append(convert(*i));
            ++i;
        }
    }
    
    return ret;
}

QVariant convert(variant::Variant in)
{
    QVariant ret;
    
    if (in.type()==variant::Type::Int32) {
        ret=QVariant(in.get_int32());
    }
    
    if (in.type()==variant::Type::Double) {
        ret=QVariant(in.get_double());
    }
    
    return ret;
}