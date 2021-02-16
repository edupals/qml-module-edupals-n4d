import QtQuick 2.6
import QtQuick.Controls 2.6 as QQC2

import Edupals.N4D 1.0 as N4D

Item {
    
N4D.Client
{
    id: client
    address: "https://localhost:9779"
    
    user: "quique"
    password: txtPass.text
    
    credential: N4D.Client.Password
}

N4D.Proxy
{
    id: get_variable
    client: client
    //plugin: "RepoManager"
    method: "get_variable"
    
    onError: {
        console.log("error:",what)
    }
    
    onResponse: {
        console.log("response:",value)
        for (var v in value) {
            console.log(v);
        }
        console.log(value)
    }
}
    
QQC2.Pane
{
    
    width: 400
    height: 300
    
    Column {
        Row {
            id: rowUser
            //anchors.horizontalCenter:parent.horizontalCenter
            
            QQC2.Label {
                text:"User"
            }
            
            QQC2.TextField {
            }
        }
        
        Row {
            id:rowPassword
            //anchors.horizontalCenter:parent.horizontalCenter
            
            QQC2.Label {
                text:"Password"
            }
            
            QQC2.TextField {
                id: txtPass
                echoMode: TextInput.Password
            }
        }
        
        Row {
            id: rowButtons
            anchors.topMargin: units.largeSpacing * 4
            anchors.right: parent.right
            QQC2.Button {
                text:"Cancel"
            }
            
            QQC2.Button {
                text:"Login"
                
                onClicked: {
                    
                    get_variable.call(["patata",true]);
                }
            }
            
        }
    }
    
    Component.onCompleted: {
        //get_info.call([1,"dos",3.2,{"a":32,"b":64,"c":128},[1,2,3,4]])
        //list_sources.call([])       
    }
}
}
