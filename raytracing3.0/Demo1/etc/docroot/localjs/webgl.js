if (!Detector.webgl) {
    Detector.addGetWebGLMessage();
}

var container;
var camera, scene, renderer;
//用于轨道控制器
var orbitControls, clock, delta;
//loadXMLDoc();
var mouse, raycaster, isShiftDown = false;
var rollOverMesh, rollOverMaterial, rollOverGeo, rollOverLine;
var triangle = [];
var distanceText = [];
var cubeMesh;
var objects = []; //存放平面，用于投影检测
var builidings = [];  //存放建筑物
var tx = [], rx = [];
var vehicles = [];
var pathLines = [];
var roadLines = [];
var roadNames = [];
var rollOverLines = [];
var xmax, xmin, ymax, ymin;
//被除数
var dividend;

var mapLenth;
var mapwidth;

var TxDTO;
var RxDTO;


var lut;

var layers = {
    plane: true,
    building: true,
    road: true,
    vehicles: true,
    ray: true,
};


main();
render();
loadPathLoss([1, 2, 3, 4, 5, 6], [-100, -101, -102, -103, -99, -98]);



function processVehicleForm() {
    var speed = document.getElementById("speed").value;
    var type1 = document.getElementById("gridRadios1");
    var type2 = document.getElementById("gridRadios2");
    var type3 = document.getElementById("gridRadios3");
    var dynamic = document.getElementById("dynamic");
    var isDynamic = dynamic.checked;
    var type;
    if (type1.checked == true) {
        type = "tx";
    } else if (type2.checked == true) {
        type = "other";
    } else if (type3.checked == true) {
        type = "rx";
    }
    console.log(speed);
    console.log(type);
    console.log(isDynamic);
    setVehicle(speed, type, isDynamic);
}





function setVehicle(speed, type, dynamic) {

    vehicleDTO.type = "vehicle";
    vehicleDTO.vehicleType = type;
    vehicleDTO.speed = speed;
    vehicleDTO.dynamic = dynamic;
    rollOverGeo = new THREE.BoxBufferGeometry(0.5, 0.5, 0.5);
    rollOverMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000, opacity: 0.5, transparent: true });
    rollOverMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
    var geometry = new THREE.LineGeometry();
    geometry.setPositions([0, 0, 0, 0, 0, 0]);
    // geometry.setColors( colors );
    var matLine = new THREE.LineMaterial({
        color: "#222221",
        linewidth: 0.002,
        // vertexColors: THREE.VertexColors,
        dashed: true
    });
    rollOverLine = new THREE.Line2(geometry, matLine);
    rollOverLine.computeLineDistances();
    rollOverLine.scale.set(1, 1, 1);
    scene.add(rollOverLine);
    rollOverLine.name = Math.random().toString(36).substr(2);
    rollOverLines.push(rollOverLine);


    scene.add(rollOverMesh);
    raycaster = new THREE.Raycaster();
    mouse = new THREE.Vector2();
    vehicleType = type;
    dynamicMod = dynamic;
    count = 0;
    startListen();

}


var vehicleType;
var dynamicMod;
var count;
var startPoint;
var distance;
var vehicleDTO = {};

function startListen() {
    orbitControls.enabled = false;
    document.addEventListener('mousemove', onDocumentMouseMove, false);
    document.addEventListener('mousedown', onDocumentMouseDown, false);
}
function stopListen() {
    vehicleType = undefined;
    dynamicMod = undefined;
    count = 0;
    startPoint = undefined;
    distance = 0;
    vehicleDTO = {};
    orbitControls.enabled = true;
    document.removeEventListener('mousemove', onDocumentMouseMove, false);
    document.removeEventListener('mousedown', onDocumentMouseDown, false);

}

function drawTriangle(intersect) {
    var triangleShape = new THREE.Shape();
    triangleShape.moveTo(0, 0);
    triangleShape.lineTo(0.5, 1);
    triangleShape.lineTo(1, 0.5);
    triangleShape.lineTo(0, 0); // close path
    var geometry = new THREE.ShapeBufferGeometry(triangleShape);
    var mesh = new THREE.Mesh(geometry, new THREE.MeshPhongMaterial({ side: THREE.DoubleSide, color: "#000000" }));

    console.log(intersect.point.x);
    console.log(startPoint.y);
    console.log(intersect.point.z);
    console.log(startPoint.x);
    var angle = Math.atan((intersect.point.x - startPoint.y)
        / (intersect.point.z - startPoint.x));
    if (intersect.point.z - startPoint.x < 0) {
        angle += Math.PI;
    }
    console.log(angle);
    mesh.position.copy(intersect.point);
    mesh.position.y = 0.1;
    mesh.rotation.set(Math.PI / 2, 0, Math.PI * 5 / 4);
    mesh.rotateZ(2 * Math.PI - angle);
    mesh.scale.set(2, 2, 2);
    mesh.name = Math.random().toString(36).substr(2);
    scene.add(mesh);
    triangle.push(mesh);
}

//将其变换到[-40,40]
function coordinateTransX(x) {
    return (x - (xmax + xmin) / 2) / (dividend) * 100
}
function coordinateTransY(y) {
    return (y - (ymax + ymin) / 2) / (dividend) * 100
}

function xToCoordinate(x) {
    return x / 100 * (dividend) + (xmax + xmin) / 2;
}
function yToCoordinate(y) {
    return y / 100 * (dividend) + (ymax + ymin) / 2;
}
function onDocumentMouseMove(event) {
    event.preventDefault();
    mouse.set((event.clientX / window.innerWidth) * 2 - 1, - (event.clientY / window.innerHeight) * 2 + 1);
    raycaster.setFromCamera(mouse, camera);
    var intersects = raycaster.intersectObjects(objects);
    var intersect;
    if (intersects.length > 0) {
        intersect = intersects[0];
        rollOverMesh.position.copy(intersect.point).add(intersect.face.normal);
        //rollOverMesh.position.divideScalar( 50 ).floor().multiplyScalar( 50 ).addScalar( 25 );
    }
    var canvas = document.getElementById("locationCanvas");
    var cxt = canvas.getContext("2d");
    cxt.font = "15px Arial";
    cxt.clearRect(0, 0, 200, 40);
    var x = intersect.point.z;
    var y = intersect.point.x;
    var longitude = xToCoordinate(x);
    var latitude = yToCoordinate(y);
    cxt.fillText(x, 10, 18);
    cxt.fillText(y, 10, 33);

    if (startPoint != undefined) {

        console.log(distance);
        var positions = [];
        positions.push(y, 0.1, x);
        positions.push(startPoint.y, 0.1, startPoint.x);
        rollOverLine.geometry.setPositions(positions);
    }
    //render();
}
//设置TX
function onDocumentMouseDown(event) {
    event.preventDefault();
    console.log(event.button);
    if (event.button == 2) {
        scene.remove(rollOverMesh);
        stopListen();
        return;
    }
    mouse.set((event.clientX / window.innerWidth) * 2 - 1, - (event.clientY / window.innerHeight) * 2 + 1);
    raycaster.setFromCamera(mouse, camera);
    var intersects = raycaster.intersectObjects(objects);
    if (intersects.length > 0) {
        var intersect = intersects[0];
        count += 1;
        scene.remove(rollOverMesh);
        var x = intersect.point.z;
        var y = intersect.point.x;
        console.log("TX location", x, y);
        var longitude = xToCoordinate(x);
        var latitude = yToCoordinate(y);

        if (dynamicMod == undefined || dynamicMod == false) {
            if (vehicleType == "other") {
                loadFbx(intersect, vehicleType)
            } else {
                load3DS(intersect, vehicleType);
            }
            vehicleDTO.location1 = [x, y];//
            console.log(vehicleDTO);
            sendJSONDataToBack("vehicle", vehicleDTO);
            console.log(vehicleDTO.vehicleType);
            if (vehicleDTO.vehicleType == 'rx') {
                $("#RXCoordinate").html("(" + x + "," + y + ")");
            } else if (vehicleDTO.vehicleType == 'tx') {
                $("#TXCoordinate").html("(" + x + "," + y + ")");
            }
            //var str = JSON.stringify(vehicleDTO);
            //sendMessage(str);
            stopListen();
            

        } else {
            if (count == 1) {
                if (vehicleType == "other") {
                    loadFbx(intersect, vehicleType)
                } else {
                    load3DS(intersect, vehicleType);
                }
                startPoint = { x: x, y: y, longitude: longitude, latitude: latitude };
                vehicleDTO.location1 = [longitude, latitude];
            } else {
                distance = GetDistance(longitude, latitude, startPoint.longitude, startPoint.latitude);
                console.log(distance);
                drawText(ENfont, distance + "m", (x + startPoint.x) / 2, (y + startPoint.y) / 2, 2);
                drawTriangle(intersect);
                vehicleDTO.location2 = [longitude, latitude];
                vehicleDTO.distance = distance;
                console.log(vehicleDTO);
                var str = JSON.stringify(vehicleDTO);
                sendMessage(str);
                stopListen();
            }

        }
    }

}

function GetDistance2(x1, y1, x2, y2) {
    var lng1 = xToCoordinate(x1);
    var lat1 = yToCoordinate(y1);
    var lng2 = xToCoordinate(x2);
    var lat2 = yToCoordinate(y2);
    return GetDistance(lng1, lat1, lng2, lat2);
}

function GetDistance(lng1, lat1, lng2, lat2) {
    var radLat1 = rad(lat1);
    var radLat2 = rad(lat2);
    var a = radLat1 - radLat2;
    var b = rad(lng1) - rad(lng2);
    var s = 2 * Math.asin(Math.sqrt(Math.pow(Math.sin(a / 2), 2) +
        Math.cos(radLat1) * Math.cos(radLat2) * Math.pow(Math.sin(b / 2), 2)));
    s = s * 6378.137;// EARTH_RADIUS;
    s = Math.round(s * 10000) / 10000;
    return s * 1000;
}

// 118.791 32.0528 118.797 32.0576
function rad(x) {
    return x * Math.PI / 180;
}


function drawSenario(senario, mod) {
    var pointXY = [];
    var pointZ = [];
    for (var i = 0; i < senario.features.length; i++) {
        var pointXXYY = [];
        var coords = senario.features[i].coordinates;
        for (var j = 0; j < coords.length; j++) {
            var x = coords[j][0];
            var y = coords[j][1];
            pointXXYY.push([coordinateTransX(x), coordinateTransY(y)]);
        }
        pointXY.push(pointXXYY);
        pointZ.push((senario.features[i].z) / 80 * (118.813658 - 118.728997) / (dividend));

    }
    layers.building = true;
    camera.layers.enable(1);
    builidings = [];
    for (var i = 0; i < pointXY.length; i++) {
        addMulGeometryNew(pointXY[i], pointZ[i], mod);
        console.log(pointXY[i]);
    }
    //console.log(pointXY);//[[]]
    gui.add(layers, 'building').onChange(function () {
        camera.layers.toggle(1);
    });
    //这儿把数据发送过去，只需要发送
    //point=[[x1,x2,x3,x4],[x1,x2,x3],[]];
    //pointY=[[y1,y2,y3,y4],[y1,y2,y3],[]];
    var mapData = {
        type: "mapData",
        data: pointXY 
    };
    var str = JSON.stringify(mapData);
    sendMessage(str);
    console.log(mapData);
}
function setTx(type) {
    orbitControls.enabled = false;
    // roll-over helpers
    rollOverGeo = new THREE.BoxBufferGeometry(0.5, 0.5, 0.5);
    rollOverMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000, opacity: 0.5, transparent: true });
    rollOverMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
    scene.add(rollOverMesh);
    raycaster = new THREE.Raycaster();
    mouse = new THREE.Vector2();
    vehicleType = type;
    vehicleDTO.type = "vehicle";
    vehicleDTO.vehicleType = type;
    vehicleDTO.dynamic = false;
    document.addEventListener('mousemove', onDocumentMouseMove, false);
    document.addEventListener('mousedown', onDocumentMouseDown, false);
}
function hide(objs) {
    for (let i = 0; i < objs.length; i++) {
        scene.remove(scene.getObjectByName(objs[i].name));
    }
}
function remove(objs) {
    hide(objs);
    var len = objs.length;
    objs = [];
}
function resume(objs) {
    if (objs.size == 0) {
        return;
    }
    for (let i = 0; i < objs.length; i++) {
        scene.add(objs[i]);
    }
}
function reset() {
    remove(tx);
    remove(rx);
    remove(pathLines);
    remove(triangle);
    remove(distanceText);
    remove(rollOverLines);
    container2.parentNode.removeChild(container2);
}



function onWindowResize() {
    camera.aspect = window.innerWidth / window.innerHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(window.innerWidth, window.innerHeight);
}


function drawRoad(roads) {
    var roadName = "init";
    roadLines = [];
    for (let i = 0; i < roads.length; i++) {
        var geometry = new THREE.Geometry();
        var material = new THREE.LineBasicMaterial({ vertexColors: true });
        var color1 = new THREE.Color("#FFFFFF");
        var color2 = new THREE.Color("#FFFFFF");
        var coordinates = roads[i].coordinates;
        var name = roads[i].name;
        var direction = roads[i].direction;
        var mid = Math.floor(coordinates.length / 2);
        var pm = coordinates[0];
        // console.log(mid);
        var x = pm[0];
        var y = pm[1];
        x = coordinateTransX(x);
        y = coordinateTransY(y);
        // console.log(x);
        // console.log(y);
        for (let j = 0; j < coordinates.length - 1; j++) {
            var p1 = coordinates[j], p2 = coordinates[j + 1];
            var x1 = coordinateTransX(p1[0]);
            var y1 = coordinateTransY(p1[1]);
            var x2 = coordinateTransX(p2[0]);
            var y2 = coordinateTransY(p2[1]);
            var q1 = new THREE.Vector3(x1, 0.1, y1);
            var q2 = new THREE.Vector3(x2, 0.1, y2);
            geometry.vertices.push(q1);
            geometry.vertices.push(q2);
            geometry.colors.push(color1, color2);
            var line = new THREE.Line(geometry, material, THREE.LineSegments);
            line.layers.set(2);
            line.name = Math.random().toString(36).substr(2);
            scene.add(line);
            roadLines.push(line);
        }
        if (name != roadName) {
            // drawText(CNfont,name,x,y,direction);
            // roadName = name;
        }
    }
    camera.layers.enable(2);
    gui.add(layers, 'road').onChange(function () {
        camera.layers.toggle(2);
    });

}

function drawText(font, text, x, y, direction) {

    // Get text from hash

    var theText = text;

    if (text == undefined) {
        return
    }
    var hash = document.location.hash.substr(1);

    if (hash.length !== 0) {

        theText = hash;

    }

    var geometry = new THREE.TextGeometry(theText, {
        font: font,
        size: 0.5,
        height: 0,
        weight: 'bold'
    });

    geometry.computeBoundingBox();

    var materials = [
        new THREE.MeshBasicMaterial({ color: 0x000000, overdraw: 0.5 }),
        new THREE.MeshBasicMaterial({ color: 0x000000, overdraw: 0.5 })
    ];

    var mesh = new THREE.Mesh(geometry, materials);

    mesh.position.x = y;
    mesh.position.y = 0.1;
    mesh.position.z = x;

    mesh.rotation.x = Math.PI / 2;
    mesh.rotation.y = Math.PI;

    if (direction == "2") {
        mesh.rotation.z = Math.PI / 2;
    }
    mesh.layers.set(2);
    camera.layers.enable(2);
    mesh.name = Math.random().toString(36).substr(2);
    if (font == ENfont) {
        distanceText.push(mesh);
    } else {
        roadNames.push(mesh);
    }
    scene.add(mesh);
}
function drawPath(data) {
    initColorMap(-200, -70);
    var paths = data.paths;
    for (var p in paths) {
        var path = paths[p];
        var nodeList = path.nodeList;
        var geometry = new THREE.Geometry();
        var material = new THREE.LineBasicMaterial({ vertexColors: true });
        var color1 = new THREE.Color(0x444444), color2 = new THREE.Color(0xFF0000);
        var color = lut.getColor(path.pathloss);
        console.log(color);
        for (let i = 0; i < nodeList.length; i++) {
            nodeList[i].x = coordinateTransX(nodeList[i].x);
            nodeList[i].y = coordinateTransY(nodeList[i].y);
        }
        for (let i = 0; i < nodeList.length - 1; i++) {
            var p1 = new THREE.Vector3(nodeList[i].y, 0.3, nodeList[i].x);
            var p2 = new THREE.Vector3(nodeList[i + 1].y, 0.3, nodeList[i + 1].x);
            geometry.vertices.push(p1);
            geometry.vertices.push(p2);
            geometry.colors.push(color, color);
            var line = new THREE.Line(geometry, material, THREE.LineSegments);
            line.name = Math.random().toString(36).substr(2);
            scene.add(line);
            line.layers.set(4);
            pathLines.push(line);
        }
    }
    camera.layers.enable(4);
    gui.add(layers, 'ray').onChange(function () {
        camera.layers.toggle(4);
    });
}



// 3Dmodel
function loadFbx(intersect, type) {
    console.log("ok");
    console.log(intersect.point);
    console.log(type);
    var x = intersect.point.z;
    var y = intersect.point.x;
    var height = 0.6;
    var length = 0.5;
    var width = 2.1;
    var point1 = {}, point2 = {}, point3 = {}, point4 = {}, point5 = {};
    var positions = [];
    var x1 = x + length / 2;
    var y1 = y + width / 2;

    var x2 = x - length / 2;
    var y2 = y + width / 2;


    var x3 = x - length / 2;
    var y3 = y - width / 2;


    var x4 = x + length / 2;
    var y4 = y - width / 2;


    point1.x = xToCoordinate(x1);
    point2.x = xToCoordinate(x2);
    point3.x = xToCoordinate(x3);
    point4.x = xToCoordinate(x4);
    point5.x = xToCoordinate(x1);

    point1.y = yToCoordinate(y1);
    point2.y = yToCoordinate(y2);
    point3.y = yToCoordinate(y3);
    point4.y = yToCoordinate(y4);
    point5.y = yToCoordinate(y1);



    vehicleDTO.bbox = [];
    vehicleDTO.bbox.push(point1, point2, point3, point4, point5);
    vehicleDTO.height = height;

    var fbxloader = new THREE.FBXLoader();
    fbxloader.load('bus/bus.fbx', function (object) {

        object.traverse(function (child) {
            if (child.isMesh) {
                child.castShadow = true;
                child.receiveShadow = true;
            }
        });
        object.scale.set(0.0002, 0.0002, 0.0002);
        object.position.copy(intersect.point);
        object.position.y = 0.3;
        // object.rotateX(Math.PI * 6 / 4);
        // object.rotateY(camera.rotation._z);
        object.rotateY(Math.PI / 2);
        object.name = Math.random().toString(36).substr(2);
        scene.add(object);
        if (type == "tx") {
            tx.push(object);
        } else {
            rx.push(object);
        }
    });

}

function load3DS(intersect, type) {
    var loader = new THREE.TDSLoader();
    loader.load("./Evo.3ds", function (object) {
        object.traverse(function (child) {
            if (child instanceof THREE.Mesh) {
                child.material.side = THREE.DoubleSide;
                //child.material.normalMap = normal;
            }
        });
        object.scale.set(0.001, 0.001, 0.001);
        object.position.copy(intersect.point);
        object.rotateX(Math.PI * 6 / 4);
        object.rotateZ(camera.rotation._z);
        // object.rotateZ(camera.rotation._z);
        object.rotateZ(Math.PI);
        object.name = Math.random().toString(36).substr(2);
        // object.layers.set(3);
        // camera.layers.enable( 3 );
        scene.add(object);
        if (type == "tx") {
            tx.push(object);
        } else if (type == "rx") {
            rx.push(object);
        }
    });
}

var container2;
var scene2;
var lut;
var renderer2;
var camera2;

var gui;
//创建颜色板
function initColorMap(min, max) {
    container2 = document.createElement('colorMap');
    document.body.appendChild(container2);
    var width = 120;
    var height = 300;
    scene2 = new THREE.Scene(); //创建一个新场景
    //添加一个透视相机
    // CAMERA
    camera2 = new THREE.PerspectiveCamera(16, width / height, 1, 100);
    camera2.position.set(0, 0, 10);
    //渲染
    //antialias:true增加抗锯齿效果
    renderer2 = new THREE.WebGLRenderer({
        antialias: true,
        alpha: true
    });
    var colorMap = 'rainbow';
    var numberOfColors = 512;
    lut = new THREE.Lut(colorMap, numberOfColors);
    lut.setMax(max);
    lut.setMin(min);
    var legend = lut.setLegendOn({ 'position': { 'x': -0.3, 'y': -0.1, 'z': -5 } });
    var labels = lut.setLegendLabels({ 'title': 'PathLoss', 'um': 'dB', 'ticks': 5 });
    // renderer2.setClearColor(new THREE.Color(0xffffff)); //设置窗口背景颜色为黑
    renderer2.setClearAlpha(0);
    renderer2.setSize(width, height); //设置窗口尺寸
    //将renderer关联到container，这个过程类似于获取canvas元素
    container2.appendChild(renderer2.domElement);
    container2.style.position = "absolute";
    container2.style.bottom = "100px";
    container2.style.right = "30px";
    scene2.add(legend);
    scene2.add(labels['title']);
    for (var i = 0; i < Object.keys(labels['ticks']).length; i++) {
        scene2.add(labels['ticks'][i]);
        scene2.add(labels['lines'][i]);
    }
    renderer2.render(scene2, camera2);
}

// main
function main() {
    //添加一个div元素
    container = document.getElementById('webgl');
    //document.body.appendChild(container);
    var width = window.innerWidth;
    var height = window.innerHeight;
    scene = new THREE.Scene(); //创建一个新场景
    //添加一个透视相机
    camera = new THREE.PerspectiveCamera(30,
        width / height, 1, 1000);
    camera.position.set(-100, 300, -100); //设置相机位置
    camera.lookAt(new THREE.Vector3(0, 0, 0)); //让相机指向原点
    //渲染
    //antialias:true增加抗锯齿效果
    renderer = new THREE.WebGLRenderer({
        antialias: true
    });
    renderer.setClearColor(new THREE.Color(0xffffff)); //设置窗口背景颜色为黑
    renderer.setSize(width, height); //设置窗口尺寸
    //将renderer关联到container，这个过程类似于获取canvas元素
    container.appendChild(renderer.domElement);

    //添加轨道控制器
    // 新建一个轨道控制器
    orbitControls = new THREE.OrbitControls(camera, renderer.domElement);
    orbitControls.target = new THREE.Vector3(0, 0, 0); //控制焦点
    orbitControls.autoRotate = false; //将自动旋转关闭
    clock = new THREE.Clock(); //用于更新轨道控制器
    //给场景添加光源
    //自然光
    var ambientLight = new THREE.AmbientLight(0x606060);
    scene.add(ambientLight);
    //平行光源
    var directionalLight = new THREE.DirectionalLight(0xffffff);
    directionalLight.position.set(1, 0.75, 0.5).normalize();
    scene.add(directionalLight);

    var nav = document.getElementById("gui");
    gui = new dat.GUI();
    var guiDomElement = gui.domElement;
    // guiDomElement.style.position = "relative";
    // guiDomElement.style.top = "0px";
    // guiDomElement.style.right = "10px";
    // guiDomElement.style.zIndex = "20";
    nav.appendChild(guiDomElement);
    // container.appendChild(guiDomElement);

    plane();

    // var gridHelper = new THREE.GridHelper( 80, 30 );
    // scene.add( gridHelper );
    window.addEventListener('resize', onWindowResize, false);
    // dragControlInit();
    //  var gridHelper = new THREE.GridHelper( 50, 100 );
    //  scene.add( gridHelper );
}

function plane() {
    var geometry = new THREE.PlaneBufferGeometry(100, 100);
    geometry.rotateX(- Math.PI / 2);
    var plane = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({ color: "#88918c", visible: true }));
    plane.layers.set(0);
    gui.add(layers, 'plane').onChange(function () {
        camera.layers.toggle(0);
    });
    scene.add(plane);
    objects.push(plane);
}



// cube2();
var cubeMesh2;
function cube2() {
    var height = 0.6;
    var length = 0.5;
    var width = 2.1;
    var cubeGeo = new THREE.CubeGeometry(length, height, width, 1, 1, 1); //创建立方体
    var cubeMat = new THREE.MeshLambertMaterial({ //创建材料
        color: 0x003300,
        wireframe: true
    });
    cubeMesh2 = new THREE.Mesh(cubeGeo, cubeMat); //创建立方体网格模型
    cubeMesh2.position.y = height / 2;

    scene.add(cubeMesh2); //将立方体添加到场景中
}

//创建一个立方体
function cube(point) {
    //THREE.CubeGeometry(width,height,depth,widthSegments,heightSegments, depthSegments)
    var height = 0.6;
    var lenght = 2.1;
    var width = 0.5;
    var cubeGeo = new THREE.CubeGeometry(lenght, height, width, 1, 1, 1); //创建立方体
    var cubeMat = new THREE.MeshLambertMaterial({ //创建材料
        color: 0x003300,
        wireframe: true
    });
    cubeMesh = new THREE.Mesh(cubeGeo, cubeMat); //创建立方体网格模型
    // cubeMesh.rotateY(camera.rotation._z);
    // cubeMesh.rotateY(Math.PI);
    cubeMesh.position.copy(point); //设置立方体的坐标 几何中心
    cubeMesh.position.y = height / 2;
    scene.add(cubeMesh); //将立方体添加到场景中
}
//创建一个球
function sphere() {
    var sphereGeo = new THREE.SphereGeometry(16, 40, 40); //创建球体
    var sphereMat = new THREE.MeshLambertMaterial({ //创建材料
        color: 0x0000FF,
        wireframe: false
    });
    var sphereMesh = new THREE.Mesh(sphereGeo, sphereMat); //创建球体网格模型
    sphereMesh.position.set(-25, 16, 0); //设置球的坐标
    scene.add(sphereMesh); //将球体添加到场景
}

// 创建建筑
function addMulGeometryNew(arrPoint, height, mod) {
    console.log("log Here");
    var arrLength = arrPoint.length;
    var city = [];
    for (var i = 0; i < arrLength; i++) {
        city.push(new THREE.Vector2(arrPoint[i][1], arrPoint[i][0]));//[x,y]
    }
    var depth = 0;
    if (mod == "3d") {
        depth = height;
    }
    var extrudeSettings = { depth: depth, bevelEnabled: false };
    var cityShape = new THREE.Shape(city);
    var points = cityShape.getPoints();

    var geometryPoints = new THREE.BufferGeometry().setFromPoints(points);
    // solid line
    var line = new THREE.Line(geometryPoints, new THREE.LineBasicMaterial({ color: 0x8DEEEE }));

    var particles = new THREE.Points(geometryPoints, new THREE.PointsMaterial({ color: '081469', size: 0.5 }));


    var geometry = new THREE.ExtrudeBufferGeometry(cityShape, extrudeSettings);
    var material = new THREE.MeshLambertMaterial({ //创建材料
        color: "#7e7d80",
        wireframe: false
    });
    var mesh = new THREE.Mesh(geometry, material);
    mesh.rotation.x = -Math.PI / 2;
    mesh.rotation.z = -Math.PI / 2;
    mesh.name = "bd" + Math.random().toString(36).substr(2);
    builidings.push(mesh);

    line.rotation.x = -Math.PI / 2;
    line.rotation.z = -Math.PI / 2;

    particles.rotation.x = -Math.PI / 2;
    particles.rotation.z = -Math.PI / 2;
    mesh.layers.set(1);
    mesh.name = Math.random().toString(36).substr(2);
    scene.add(mesh);
    builidings.push(mesh);
}
function addMulGeometry(arrZ, arrX, height, mod) {
    var arrLength = arrX.length;
    var city = [];
    for (var i = 0; i < arrLength; i++) {
        city.push(new THREE.Vector2(arrZ[i], arrX[i]));
    }
    var depth = 0;
    if (mod == "3d") {
        depth = height;
    }
    var extrudeSettings = { depth: depth, bevelEnabled: false };
    var cityShape = new THREE.Shape(city);
    var points = cityShape.getPoints();

    var geometryPoints = new THREE.BufferGeometry().setFromPoints(points);
    // solid line
    var line = new THREE.Line(geometryPoints, new THREE.LineBasicMaterial({ color: 0x8DEEEE }));

    var particles = new THREE.Points(geometryPoints, new THREE.PointsMaterial({ color: '081469', size: 0.5 }));


    var geometry = new THREE.ExtrudeBufferGeometry(cityShape, extrudeSettings);
    var material = new THREE.MeshLambertMaterial({ //创建材料
        color: "#7e7d80",
        wireframe: false
    });
    var mesh = new THREE.Mesh(geometry, material);
    mesh.rotation.x = -Math.PI / 2;
    mesh.rotation.z = -Math.PI / 2;
    mesh.name = "bd" + Math.random().toString(36).substr(2);
    builidings.push(mesh);

    line.rotation.x = -Math.PI / 2;
    line.rotation.z = -Math.PI / 2;

    particles.rotation.x = -Math.PI / 2;
    particles.rotation.z = -Math.PI / 2;
    mesh.layers.set(1);
    mesh.name = Math.random().toString(36).substr(2);
    scene.add(mesh);
    builidings.push(mesh);
}


//渲染
function render() {
    delta = clock.getDelta();
    orbitControls.update(delta);
    requestAnimationFrame(render);
    renderer.render(scene, camera);
}

//画线段
function drawline(positions) {
    var geometry = new THREE.LineGeometry();
    geometry.setPositions(positions);
    // geometry.setColors( colors );
    var matLine = new THREE.LineMaterial({
        color: "#aa8f13",
        linewidth: 0.002,
        // vertexColors: THREE.VertexColors,
        dashed: false
    });
    line = new THREE.Line2(geometry, matLine);
    line.computeLineDistances();
    line.scale.set(1, 1, 1);
    scene.add(line);
}
function addLines(datas,colorS) {
    var geometry = new THREE.Geometry();//声明一个空几何体对象
    for (var i = 0; i < datas.length; i++) {
        geometry.vertices.push(new THREE.Vector3(datas[i][0], 0.1, datas[i][1]));
    }
    var material = new THREE.LineBasicMaterial({
        color: colorS //线条颜色
    });//材质对象
    var line = new THREE.Line(geometry, material);//线条模型对象
    scene.add(line);//线条对象添加到场景中
}
function drawMyMaps(data){
    var mymaps=data.data;
    for(var p in mymaps){
        addLines([[mymaps[p][0]-50,mymaps[p][1]-50],[mymaps[p][2]-50,mymaps[p][3]-50]],0x00ff00);
    }
}
function drawMyLine(data) {
    var mypaths = data.paths;

    for (var path in mypaths) {
        console.log(path);
        var positionPath = mypaths[path].nodeList;
        var MyPositionS = [];
        for (var p in positionPath) {

            //console.log(positionPath[p].x,positionPath[p].y);
            console.log(positionPath[p].x, positionPath[p].y);
            MyPositionS.push([positionPath[p].x, positionPath[p].y]);
        }
        // var geometry = new THREE.Geometry();
        // var material = new THREE.LineBasicMaterial({ vertexColors: true });
        // var color1 = new THREE.Color(0x444444), color2 = new THREE.Color(0xFF0000);
        // for(var i=0;i<MyPositionS.length-1;i++){
        //     var p1 = new THREE.Vector3(MyPositionS[i][0], 0.3, MyPositionS[i][1]);
        //     var p2 = new THREE.Vector3(MyPositionS[i + 1][0], 0.3, MyPositionS[i + 1][1]);
        //     geometry.vertices.push(p1);
        //     geometry.vertices.push(p2);
        //     geometry.colors.push(color1,color2);
        //     var line = new THREE.Line(geometry, material, THREE.LineSegments);
        //     line.name = Math.random().toString(36).substr(2);
        //     scene.add(line);
        //     line.layers.set(4);
        //     pathLines.push(line);
        // }
        addLines(MyPositionS,0xff0000);
    }
}
function drawMapOutlines(data){
    var outlines=data.data;
    console.log(outlines);
    for(var outline in outlines){
        var outli=outlines[outline];
        var MyPositionS = [];
        MyPositionS.push([outli[0],outli[1]]);
        MyPositionS.push([outli[2],outli[3]]);
        addLines(MyPositionS,0xffff00);
        // console.log(outli);
        // console.log(outli[0],outli[1],outli[2],outli[3]);
    }
}
//addLines([[20, 20], [20, 0]]);//[[y,x]]
addLines([[0, 0], [50, 0]], "#ffff00");
addLines([[25, 0], [25, 3]], "#ffff00");
addLines([[0, 20], [3, 20]], "#ffff00");
addLines([[0, 30], [3, 30]], "#ffff00");

addLines([[0, 0], [0, 50]], "#ffff00");
// var testPosition=[[-50,-50],[50,-50],[50,50],[-50,50]];
// addLines(testPosition);
// testPosition=[[-2.2786273650064914 -7.033614483760113],[13.37613193646929 -39.13063479757311]];
// addLines(testPosition);

var positions = [];
positions.push(-40, 0.5, -40);
positions.push(40, 0.5, -40);
positions.push(40, 0.5, 40);
positions.push(-40, 0.5, 40);
positions.push(-40, 0.5, -40);
drawline(positions);













//
// function onDocumentKeyDown(event) {
//     switch (event.keyCode) {
//         case 16: isShiftDown = true; break;
//     }
// }
// function onDocumentKeyUp(event) {
//     switch (event.keyCode) {
//         case 16: isShiftDown = false; break;
//     }
// }

// function addGeometry() {
//     var geometry = new THREE.Geometry();
//     // 设置顶点位置
//     // 顶部4顶点
//     geometry.vertices.push(new THREE.Vector3(-1, 2, -1));
//     geometry.vertices.push(new THREE.Vector3(1, 2, -1));
//     geometry.vertices.push(new THREE.Vector3(1, 2, 1));
//     geometry.vertices.push(new THREE.Vector3(-1, 2, 1));
//     // 底部4顶点
//     geometry.vertices.push(new THREE.Vector3(-2, 0, -2));
//     geometry.vertices.push(new THREE.Vector3(2, 0, -2));
//     geometry.vertices.push(new THREE.Vector3(2, 0, 2));
//     geometry.vertices.push(new THREE.Vector3(-2, 0, 2));
//
//     // 设置顶点连接情况
//     // 顶面
//     geometry.faces.push(new THREE.Face3(0, 1, 3));
//     geometry.faces.push(new THREE.Face3(1, 2, 3));
//     //          geometry.faces.push(new THREE.Face4(0, 1, 2, 3));
//     // 底面
//     geometry.faces.push(new THREE.Face3(4, 5, 6));
//     geometry.faces.push(new THREE.Face3(5, 6, 7));
//     //          geometry.faces.push(new THREE.Face4(4, 5, 6, 7));
//     // 侧面
//     geometry.faces.push(new THREE.Face3(1, 5, 6));
//     geometry.faces.push(new THREE.Face3(6, 2, 1));
//     geometry.faces.push(new THREE.Face3(2, 6, 7));
//     geometry.faces.push(new THREE.Face3(7, 3, 2));
//     geometry.faces.push(new THREE.Face3(3, 7, 0));
//     geometry.faces.push(new THREE.Face3(7, 4, 0));
//     geometry.faces.push(new THREE.Face3(0, 4, 5));
//     geometry.faces.push(new THREE.Face3(0, 5, 1));
//     var material = new THREE.MeshLambertMaterial({ //创建材料
//         color: 0xffff00,
//         wireframe: false
//     });
//     var mesh = new THREE.Mesh(geometry, material);
//     scene.add(mesh);
// }

// function addOwnGeometry() {
//     var arrX = [0, 0, 4, 4];
//     var arrY = [0, 4, 4, 0];
//     var height = 4;
//     var arrLength = arrX.length;
//     // 创建一个立方体
//     //    v6----- v5
//     //   /|      /|
//     //  v1------v0|
//     //  | |     | |
//     //  | |v7---|-|v4
//     //  |/      |/
//     //  v2------v3
//     var geometry = new THREE.Geometry();
//     //创建立方体的顶点
//     //[x,y,z]
//     //
//     //        |y
//     //        |
//     //     (0,0,0)------x
//     //       /
//     //      /z
//     var vertices = [
//         new THREE.Vector3(10, 10, 10), //v0
//         new THREE.Vector3(-10, 10, 10), //v1
//         new THREE.Vector3(-10, -10, 10), //v2
//         new THREE.Vector3(10, -10, 10), //v3
//         new THREE.Vector3(10, -10, -10), //v4
//         new THREE.Vector3(10, 10, -10), //v5
//         new THREE.Vector3(-10, 10, -10), //v6
//         new THREE.Vector3(-10, -10, -10) //v7
//     ];
//
//     geometry.vertices = vertices;
//     //创建立方的面
//     var faces = [
//         new THREE.Face3(0, 1, 2),
//         new THREE.Face3(0, 2, 3),
//         new THREE.Face3(0, 3, 4),
//         new THREE.Face3(0, 4, 5),
//         new THREE.Face3(1, 6, 7),
//         new THREE.Face3(1, 7, 2),
//         new THREE.Face3(6, 5, 4),
//         new THREE.Face3(6, 4, 7),
//         new THREE.Face3(5, 6, 1),
//         new THREE.Face3(5, 1, 0),
//         new THREE.Face3(3, 2, 7),
//         new THREE.Face3(3, 7, 4)
//     ];
//
//     geometry.faces = faces;
//     geometry.computeFaceNormals();
//     var material = new THREE.MeshLambertMaterial({ //创建材料
//         color: 0xffff00,
//         wireframe: false
//     });
//     var mesh = new THREE.Mesh(geometry, material);
//     scene.add(mesh);
// }

// function addMulGeometryText() {
//     var arrX = [67.18841944111367, 65.17594444951699, 56.737672119402966, 58.75014711099965];
//     var arrZ = [157.04366246983577, 152.91279275128568, 155.91385194907826, 160.0447216678792];
//     var height = 24;
//     var arrLength = arrX.length;
//     var geometry = new THREE.Geometry();
//     for (var i = 0; i < arrLength; i++) {
//         geometry.vertices.push(new THREE.Vector3(arrX[i], height, arrZ[i]));
//     }
//     for (var i = 0; i < arrLength; i++) {
//         geometry.vertices.push(new THREE.Vector3(arrX[i], 0, arrZ[i]));
//     }
//     for (var i = 0; i < arrLength; i++) {
//         if (i + arrLength + 1 == 2 * arrLength) {
//             geometry.faces.push(new THREE.Face3(i, (i + 1) % arrLength, arrLength));
//             geometry.faces.push(new THREE.Face3(i, arrLength, i + arrLength));
//         } else {
//             geometry.faces.push(new THREE.Face3(i, (i + 1) % arrLength, i + arrLength + 1));
//             geometry.faces.push(new THREE.Face3(i, i + arrLength + 1, i + arrLength));
//         }
//     }
//     for (var i = 0; i < arrLength - 2; i++) {
//         geometry.faces.push(new THREE.Face3(0, i + 2, i + 1));
//     }
//     for (var i = 0; i < arrLength - 2; i++) {
//         geometry.faces.push(new THREE.Face3(arrLength, arrLength + i + 1, arrLength + i + 2));
//     }
//     geometry.computeFaceNormals();
//     var material = new THREE.MeshLambertMaterial({ //创建材料
//         color: 0xffff00,
//         wireframe: false
//     });
//     var mesh = new THREE.Mesh(geometry, material);
//     scene.add(mesh);
// }

//创建圆柱体
// function cylinder() {
//     //创建圆柱体
//     var cylinderGeo = new THREE.CylinderGeometry(15, 15, 40, 40, 40);
//     var cylinderMat = new THREE.MeshLambertMaterial({ //创建材料
//         color: 0xFF6600,
//         wireframe: false
//     });
//     //创建圆柱体网格模型
//     var cylinderMesh = new THREE.Mesh(cylinderGeo, cylinderMat);
//     cylinderMesh.position.set(0, 20, -40); //设置圆柱坐标
//     scene.add(cylinderMesh); //向场景添加圆柱体
// }

//画线段



//主函数
// function loadXMLDoc() {
//     var xmlhttp;
//     if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
//         xmlhttp = new XMLHttpRequest();
//     } else { // code for IE6, IE5
//         xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
//     }
//     xmlhttp.onreadystatechange = function () {
//         if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
//             var result = xmlhttp.responseText;
//             //document.getElementById("myDiv").innerHTML=result;
//             var obj = JSON.parse(result);
//             console.log(obj[0][0].x)
//             console.log(obj[0][0].y)
//             //console.log(obj.length)
//             var pointX = [];
//             var pointY = [];
//             var pointZ = [];
//             for (var i = 0; i < 261; i++) {
//                 var pointXX = [];
//                 var pointYY = [];
//                 for (var j = obj[i].length - 1; j >= 0; j--) {
//                     pointXX.push(obj[i][j].x);
//                     pointYY.push(obj[i][j].y);
//                 }
//                 pointX.push(pointXX);
//                 pointY.push(pointYY);
//                 pointZ.push(obj[i][0].z);
//             }
//             main(pointX, pointY, pointZ);
//             render();
//             loadPathLossGet();
//
//             //alert(obj[0].length);
//         }
//     }
//     xmlhttp.open("GET", "http://localhost:8080/user/hello.html", true);
//     xmlhttp.send();
// }

//
// function loadLegends(colorMap, numberOfColors, legendLayout){
//
//     lut = new THREE.Lut( colorMap, numberOfColors );
//
//     lut.setMax( 2000 );
//     lut.setMin( 1000 );
//
//     if ( legendLayout ) {
//
//         var legend;
//
//         if ( legendLayout === 'horizontal' ) {
//
//             legend = lut.setLegendOn( { 'layout':'horizontal', 'position': { 'x': 0, 'y': -3, 'z': 3} } );
//
//         } else {
//
//             legend = lut.setLegendOn();
//
//         }
//
//         scene.add ( legend );
//
//         var labels = lut.setLegendLabels( { 'title': 'PathLoss', 'um': 'dB', 'ticks': 5 } );
//
//         scene.add ( labels['title'] );
//
//         for ( var i = 0; i < Object.keys( labels[ 'ticks' ] ).length; i ++ ) {
//
//             scene.add ( labels[ 'ticks' ][ i ] );
//             scene.add ( labels[ 'lines' ][ i ] );
//
//         }
//
//     }
// }


// function setRx() {
//
//     orbitControls.enabled = false;
//     // roll-over helpers
//     rollOverGeo = new THREE.BoxBufferGeometry(1, 0.5, 0.7);
//     rollOverMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000, opacity: 0.5, transparent: true });
//     rollOverMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
//     scene.add(rollOverMesh);
//
//     raycaster = new THREE.Raycaster();
//     mouse = new THREE.Vector2();
//     document.addEventListener('mousemove', onDocumentMouseMove, false);
//     document.addEventListener('mousedown', onDocumentMouseDown2, false);
//
// }

// function processCordinate(result) {
//     var obj = JSON.parse(result);
//     xmax = obj.xmax;
//     xmin = obj.xmin;
//     ymax = obj.ymax;
//     ymin = obj.ymin;
// }

//设置RX
// function onDocumentMouseDown2(event) {
//     event.preventDefault();
//     mouse.set((event.clientX / window.innerWidth) * 2 - 1, - (event.clientY / window.innerHeight) * 2 + 1);
//     raycaster.setFromCamera(mouse, camera);
//     var intersects = raycaster.intersectObjects(objects);
//     if (intersects.length > 0) {
//         var intersect = intersects[0];
//         loadFbx(intersect,"rx");
//         // load3DS(intersect, "rx");
//         scene.remove(rollOverMesh);
//         var x = intersect.point.z;
//         var y =  intersect.point.x;
//         var longitude = x/80*(xmax - xmin) + (xmax + xmin) / 2;
//         var latitude  = y/80*(xmax - xmin) + (ymax + ymin)  / 2;
//         RxDTO = { "type": "rx", "longitude" : longitude , "latitude" : latitude};
//         var str = JSON.stringify(RxDTO);
//         sendMessage(str);
//     }
//
//     orbitControls.enabled = true;
//     document.removeEventListener('mousemove', onDocumentMouseMove, false);
//     document.removeEventListener('mousedown', onDocumentMouseDown2, false);
//
// }

// function loadPathLossGet() {
//     var xmlhttp;
//     if (window.XMLHttpRequest) { // code for IE7+, Firefox, Chrome, Opera, Safari
//         xmlhttp = new XMLHttpRequest();
//     } else { // code for IE6, IE5
//         xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
//     }
//     xmlhttp.onreadystatechange = function () {
//         if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
//             var result = xmlhttp.responseText;
//             console.log(result);
//             //document.getElementById("myDiv").innerHTML=result;
//             var obj = JSON.parse(result);
//             console.log(obj.length);
//             var pathLossT = [];
//             var pathLossOrder = [];
//             for (var i = 0; i < obj.length; i++) {
//                 pathLossT.push(obj[i]);
//                 pathLossOrder.push(i);
//             }
//             //loadDimenPowerDelay();
//             loadPathLoss(pathLossOrder, pathLossT);
//         }
//     }
//     xmlhttp.open("GET", "http://localhost:8080/user/getPathLoss.html", true);
//     xmlhttp.send();
// }

// function readFromDataToArray(result) {
//
//     var obj = JSON.parse(result);
//     console.log(obj[0][0].x)
//     console.log(obj[0][0].y)
//     //console.log(obj.length)
//     var pointX = [];
//     var pointY = [];
//     var pointZ = [];
//
//     for (var p in obj) {
//         var data = obj[p];
//         var pointXX = [];
//         var pointYY = [];
//         for (var i = 0; i < data.length; i++) {
//             pointXX.push(data[i].x);
//             pointYY.push(data[i].y);
//         }
//
//         pointX.push(pointXX);
//         pointY.push(pointYY);
//         pointZ.push(data[0].z);
//     }
//     for (var i = 0; i < pointX.length; i++) {
//         addMulGeometry(pointX[i], pointY[i], pointZ[i]);
//     }
// }
