// <!--websocket-->
/*
我们需要统一定义发送的数据格式
我设置的发送数据格式为
var mapData={
        type:"mapData",
        data:{
        }
    };
其中type表示数据类型
data中存储具体的数据格式
*/
var sendCode;
var wsUri = "ws://localhost:1234";
var websocket = null;


function debug(message) {
    console.log(message);
}

function sendMessage(msg) {
    if (websocket != null) {
        websocket.send(msg);
        console.log("string sent :", '"' + msg + '"');
    } else {
        //initWebSocket();
        websocket.send(msg);
        console.log("string sent :", '"' + msg + '"');
    }
    sendCode = msg;
    // stopWebSocket();
}

initWebSocket();

function initWebSocket() {
    try {
        if (typeof MozWebSocket == 'function')
            WebSocket = MozWebSocket;
        if (websocket && websocket.readyState == 1)
            websocket.close();
        websocket = new WebSocket(wsUri);
        websocket.onopen = function (evt) {
            // sendMessage("1");
            debug("CONNECTED");
        };
        websocket.onclose = function (evt) {
            debug("DISCONNECTED");
        };
        websocket.onmessage = function (evt) {
            console.log("Message received :", evt.data);
            var data = JSON.parse(evt.data);
            console.log("received data type=", data.type);
            if (data.type == "output") {
                if (data.state == "1") {
                    drawMyLine(data);
                } else {

                }
            } else if (data.type == "inputMap") {
                drawMyMaps(data);
            } else if (data.type == "mapDataGet") {
                //console.log("here");
                drawMapOutlines(data);
            } else if (data.type == "updateMapData") {

            } else if (data.type == "logOutput") {
                console.log(data.data);
                $("#logOutput").append(data.data + "</br>");
            }
        };
        websocket.onerror = function (evt) {
            debug('ERROR: ' + evt.data);
        };
    } catch (exception) {
        debug('ERROR: ' + exception);
    }
}

function stopWebSocket() {
    if (websocket)
        websocket.close();
}

function checkSocket() {
    if (websocket != null) {
        var stateStr;
        switch (websocket.readyState) {
            case 0: {
                stateStr = "CONNECTING";
                break;
            }
            case 1: {
                stateStr = "OPEN";
                break;
            }
            case 2: {
                stateStr = "CLOSING";
                break;
            }
            case 3: {
                stateStr = "CLOSED";
                break;
            }
            default: {
                stateStr = "UNKNOW";
                break;
            }
        }
        debug("WebSocket state = " + websocket.readyState + " ( " + stateStr + " )");
    } else {
        debug("WebSocket is null");
    }
}
function beginRun() {
    sendDataToBack("runVPL", "");
    //var sendData={
    //    type:"runVPL",
    //    data:{

    //    }
    //}
    //sendMessage(JSON.stringify(sendData));
}
function beginRunMap() {
    sendDataToBack("runMap", "");
    //var sendData={
    //    type:"runMap",
    //    data:{

    //    }
    //}
    //sendMessage(JSON.stringify(sendData));
}
function getMapFromBack() {
    sendDataToBack("getMap", "");
    //var sendData={
    //    type:"getMap",
    //    data:{

    //    }
    //}
    //sendMessage(JSON.stringify(sendData));
}
function requireMapData() {
    sendDataToBack("requireMapData", "");
    //var sendData={
    //    type:"requireMapData",
    //    data:{

    //    }
    //}
    //sendMessage(JSON.stringify(sendData));
}
function sendDataToBack(dataType, data) {
    var sendData = {
        type: dataType,
        data: {
            data
        }
    }
    sendMessage(JSON.stringify(sendData));
}
function sendJSONDataToBack(dataType, data) {
    var sendData = {
        type: dataType,
        data
    }
    sendMessage(JSON.stringify(sendData));
}
