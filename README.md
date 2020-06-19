# qml-module-edupals-n4d
qml plugin for accessing edupals n4d rpc server

```
import Edupals.N4D 1.0 as N4D

Item {
    
N4D.Client
{
    id: client
    address: "https://localhost"
    port:9779
    user: "user"
    password: "pass"
    anonymous: false
}

N4D.Proxy
{
    id: get_info
    client: client
    plugin: "LliureXInfo"
    method: "get_info"
    
    onError: {
        console.log("error:",what)
    }
    
    onResponse: {
        console.log("response:",value)
    }
}

N4D.Proxy
{
    id: list_sources
    client: client
    plugin: "RepoManager"
    method: "list_sources"
    
    onError: {
        console.log("error:",what)
    }
    
    onResponse: {
        console.log("response:",value)
        for (var v in value["data"]) {
            console.log(v);
        }
        console.log(value["data"])
    }
}
    
Rectangle
{
    
    width: 320
    height: 240
    
    color: "red"
    
    Component.onCompleted: {
        get_info.call([1,"dos",3.2,{"a":32,"b":64,"c":128},[1,2,3,4]])
        list_sources.call([])       
    }
}
}
```
