# This project demonstrates how to use QtWebAppLib by including the
# sources into this project.

TARGET = Demo1
TEMPLATE = app
QT = core network quick webengine  websockets
HEADERS += \
           src/requestmapper.h \
           src/controller/dumpcontroller.h \
           src/controller/templatecontroller.h \
           src/controller/formcontroller.h \
           src/controller/fileuploadcontroller.h \
           src/controller/sessioncontroller.h \
    src/echoserver.h \
    src/edge.h \
    src/FileManager.h \
    src/FilePoint.h \
    src/mesh.h \
    src/node.h \
    src/object.h \
    src/path.h \
    src/point.h \
    src/scene.h \
    src/tracer.h \
    src/type.h \
    src/vehicle.h \
    src/voxel.h \
    src/tools.h

SOURCES += src/main.cpp \
           src/requestmapper.cpp \
           src/controller/dumpcontroller.cpp \
           src/controller/templatecontroller.cpp \
           src/controller/formcontroller.cpp \
           src/controller/fileuploadcontroller.cpp \
           src/controller/sessioncontroller.cpp \
    src/echoserver.cpp \
    src/FilePoint.cpp

OTHER_FILES += etc/* etc/docroot/* etc/templates/* etc/ssl/* logs/* ../readme.txt

#---------------------------------------------------------------------------------------
# The following lines include the sources of the QtWebAppLib library
#---------------------------------------------------------------------------------------

include(../QtWebApp/logging/logging.pri)
include(../QtWebApp/httpserver/httpserver.pri)
include(../QtWebApp/templateengine/templateengine.pri)
# Not used: include(../QtWebApp/qtservice/qtservice.pri)

DISTFILES += \
    etc/docroot/Evo.3ds \
    etc/docroot/index.html \
    etc/docroot/js/controls/DeviceOrientationControls.js \
    etc/docroot/js/controls/DragControls.js \
    etc/docroot/js/controls/EditorControls.js \
    etc/docroot/js/controls/FirstPersonControls.js \
    etc/docroot/js/controls/FlyControls.js \
    etc/docroot/js/controls/MapControls.js \
    etc/docroot/js/controls/OrbitControls.js \
    etc/docroot/js/controls/OrthographicTrackballControls.js \
    etc/docroot/js/controls/PointerLockControls.js \
    etc/docroot/js/controls/TrackballControls.js \
    etc/docroot/js/controls/TransformControls.js \
    etc/docroot/js/libs/draco/gltf/draco_decoder.js \
    etc/docroot/js/libs/draco/gltf/draco_encoder.js \
    etc/docroot/js/libs/draco/gltf/draco_wasm_wrapper.js \
    etc/docroot/js/libs/draco/draco_decoder.js \
    etc/docroot/js/libs/draco/draco_wasm_wrapper.js \
    etc/docroot/js/libs/ammo.js \
    etc/docroot/js/libs/dat.gui.min.js \
    etc/docroot/js/libs/gunzip.min.js \
    etc/docroot/js/libs/inflate.min.js \
    etc/docroot/js/libs/jszip.min.js \
    etc/docroot/js/libs/mmdparser.min.js \
    etc/docroot/js/libs/o3dgc.js \
    etc/docroot/js/libs/opentype.min.js \
    etc/docroot/js/libs/stats.min.js \
    etc/docroot/js/libs/system.min.js \
    etc/docroot/js/libs/timeliner_gui.min.js \
    etc/docroot/js/libs/tween.min.js \
    etc/docroot/js/bootstrap.bundle.js \
    etc/docroot/js/bootstrap.bundle.min.js \
    etc/docroot/js/bootstrap.js \
    etc/docroot/js/bootstrap.min.js \
    etc/docroot/js/ColladaLoader.js \
    etc/docroot/js/Detector.js \
    etc/docroot/js/echarts-gl.min.js \
    etc/docroot/js/echarts.min.js \
    etc/docroot/js/jquery.min.js \
    etc/docroot/js/matrix.js \
    etc/docroot/js/MTLLoader.js \
    etc/docroot/js/OBJLoader.js \
    etc/docroot/js/OrbitControls.js \
    etc/docroot/js/popper.min.js \
    etc/docroot/js/qwebchannel.js \
    etc/docroot/js/show.js \
    etc/docroot/js/TDSLoader.js \
    etc/docroot/js/tether.min.js \
    etc/docroot/js/three.js \
    etc/docroot/js/three.min.js \
    etc/docroot/js/three.module.js \
    etc/docroot/js/TrackballControls.js \
    etc/docroot/js/libs/draco/gltf/draco_decoder.wasm \
    etc/docroot/js/libs/draco/draco_decoder.wasm \
    etc/docroot/css/bootstrap-grid.css \
    etc/docroot/css/bootstrap-grid.min.css \
    etc/docroot/css/bootstrap-reboot.css \
    etc/docroot/css/bootstrap-reboot.min.css \
    etc/docroot/css/bootstrap.css \
    etc/docroot/css/bootstrap.min.css \
    etc/docroot/css/webgl.css \
    etc/docroot/js/libs/draco/README.md \
    etc/docroot/css/bootstrap-grid.css.map \
    etc/docroot/css/bootstrap-grid.min.css.map \
    etc/docroot/css/bootstrap-reboot.css.map \
    etc/docroot/css/bootstrap-reboot.min.css.map \
    etc/docroot/css/bootstrap.css.map \
    etc/docroot/css/bootstrap.min.css.map \
    etc/docroot/js/bootstrap.bundle.js.map \
    etc/docroot/js/bootstrap.bundle.min.js.map \
    etc/docroot/js/bootstrap.js.map \
    etc/docroot/js/bootstrap.min.js.map \
    etc/docroot/index.html

RESOURCES += \
    qml.qrc
