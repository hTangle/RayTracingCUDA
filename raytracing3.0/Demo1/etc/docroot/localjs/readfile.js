
var fileInput = document.getElementById('file-input');
var fileInput2 = document.getElementById('dbf-file-input');
var roadfileInput = document.getElementById('road-file-input');
var roadfileInput2 = document.getElementById('road-dbf-file-input');

var shpBuffer;
var dbfBuffer;

var features = [];
var senario = {};
var roads = [];

// 监听change事件:
fileInput.addEventListener('change', function () {


    var file = fileInput.files[0];

    if (!(file.name.endsWith('.shp') )) {
        alert('Can only upload shp file.');
        return false;
    }
    // 读取文件:
    var reader = new FileReader();
    reader.onload = function(e) {
        shpBuffer = e.target.result;

    };
    reader.readAsArrayBuffer(file);

});

roadfileInput.addEventListener('change', function () {


    var file = roadfileInput.files[0];

    if (!(file.name.endsWith('.shp') )) {
        alert('Can only upload shp file.');
        return false;
    }
    // 读取文件:
    var reader = new FileReader();
    reader.onload = function(e) {
        shpBuffer = e.target.result;

    };
    reader.readAsArrayBuffer(file);

});

fileInput2.addEventListener('change', function () {
    var file = fileInput2.files[0];


    if (!(file.name.endsWith('.dbf') )) {
        alert('Can only upload dbf file.');
        return false;
    }
    // 读取文件:
    var reader = new FileReader();
    reader.onload = function(e) {
        dbfBuffer = e.target.result;
    };
    reader.readAsArrayBuffer(file);

});

roadfileInput2.addEventListener('change', function () {
    var file = roadfileInput2.files[0];


    if (!(file.name.endsWith('.dbf') )) {
        alert('Can only upload dbf file.');
        return false;
    }
    // 读取文件:
    var reader = new FileReader();
    reader.onload = function(e) {
        dbfBuffer = e.target.result;
    };
    reader.readAsArrayBuffer(file);
});

function parseRoadFile() {

    shapefile.open(shpBuffer,dbfBuffer,{ encoding : "GB2312"})
        .then(source => source.read()
            .then(function log(result) {
                if (result.done) return;
                // console.log(result)
                var road = {
                    coordinates : result.value.geometry.coordinates,
                    name : result.value.properties.NAME
                }
                roads.push(road);
                return source.read().then(log);
            }).then(function () {
                if (xmin == undefined){
                    xmin = source.bbox[0];
                    ymin = source.bbox[1];
                    xmax = source.bbox[2];
                    ymax = source.bbox[3];
                }
            }).then(function () {
                drawRoad(roads);
            }))
        .catch(error => console.error(error.stack));
}
function parseFile() {
    remove(builidings);
    var shp = "./file/BUILDING_nanjing.shp";
    var dbf = "./file/BUILDING_nanjing.dbf";
    if(shpBuffer != undefined && dbfBuffer != undefined){
        shp = shpBuffer;
        dbf = dbfBuffer;
    }
    //读取shp时
    shapefile.open(shp,dbf)
        .then(source => source.read()
            .then(function log(result) {
                if (result.done) return;
                var feature = {
                    coordinates : result.value.geometry.coordinates[0],
                    z : result.value.properties.HEIGHT
                }
                features.push(feature);
                return source.read().then(log);

            }).then(function () {
                senario = {
                    features : features,
                    bbox : source.bbox
                }
                // if (xmin == undefined){
                xmin = source.bbox[0];
                ymin = source.bbox[1];
                xmax = source.bbox[2];
                ymax = source.bbox[3];
                // }

            }).then(function () {
                drawSenario(senario,"3d");
            }).then(function () {
                senario.type = "senario";
                var str = JSON.stringify(senario);
                console.log(str);
                sendMessage(str);
            }))
        .catch(error => console.error(error.stack));
}

function parseDefaultfile(name){
    var building = "./" + name + "/BUILDING_nanjing.shp";
    var road = "./" + name + "/R.shp";
    var roaddbf = "./" + name + "/R.dbf";
    //console.log(building);
    if(gui.domElement.children[1].childElementCount > 1){
        var nav = document.getElementById("gui");
        nav.removeChild(nav.firstElementChild);
        gui = new dat.GUI();
        var guiDomElement = gui.domElement;
        nav.appendChild(guiDomElement);

        gui.add( layers, 'plane' ).onChange( function () {
            camera.layers.toggle( 0 );
        } );
        senario = {};
        features = [];
        roads = [];
        remove(builidings);
        remove(roadLines);
        remove(roadNames);
        remove(pathLines);
    }

    shapefile.open(building)
        .then(source => source.read()
            .then(function log(result) {
                if (result.done) return;
                var feature = {
                    coordinates : result.value.geometry.coordinates[0],
                    z : result.value.properties.HEIGHT
                }
                features.push(feature);
                return source.read().then(log);

            }).then(function () {
                senario = {
                    features : features,
                    bbox : source.bbox
                }
                xmin = source.bbox[0];
                ymin = source.bbox[1];
                xmax = source.bbox[2];
                ymax = source.bbox[3];
                dividend = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
                mapLenth = GetDistance(xmin,ymin,xmin,ymax);
                mapwidth = GetDistance(xmin,ymin,xmax,ymin);
            }).then(function () {
                drawSenario(senario,"3d");
            }).then(function () {
                // senario.type = "senario";
                // var str = JSON.stringify(senario);
                // sendMessage(str);
            }))
        .catch(error => console.error(error.stack));

    shapefile.open(road,roaddbf,{ encoding : "GB2312"})
        .then(source => source.read()
            .then(function log(result) {
                if (result.done) return;

                var road = {
                    coordinates : result.value.geometry.coordinates,
                    name : result.value.properties.NAME,
                    direction : result.value.properties.DIRECTION
                }
                // console.log(road.name);
                // console.log(road.direction)
                roads.push(road);
                return source.read().then(log);
            }).then(function () {
                if (xmin == undefined){
                    xmin = source.bbox[0];
                    ymin = source.bbox[1];
                    xmax = source.bbox[2];
                    ymax = source.bbox[3];
                    dividend = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
                    mapLenth = GetDistance(xmin,ymin,xmin,ymax);
                    mapwidth = GetDistance(xmin,ymin,xmax,ymin);
                }
            }).then(function () {
                drawRoad(roads);
            }))
        .catch(error => console.error(error.stack));
}


var loader = new THREE.FontLoader();
var CNfont;
var ENfont;

function initFont() {
    loader.load( 'fonts/FZLanTingHeiS-UL-GB_Regular.json', function ( font ) {
        CNfont = font;
    } );
    loader.load( 'fonts/gentilis_regular.typeface.json', function ( font ) {
        ENfont = font;
    } );
}

initFont();

