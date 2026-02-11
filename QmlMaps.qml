import QtQuick
import QtLocation
import QtPositioning

Rectangle    {
    id:window

    property double latitude: -31.756285
    property double longitude: -52.335829

    property Component locationmarker: marker
    Plugin
    {
        id:openstreetview
        name:"osm"
    }
    Map{
        id:map
        anchors.fill: parent
        plugin: openstreetview
        center: QtPositioning.coordinate(latitude, longitude)
        zoomLevel: 13
        
        PinchHandler {
            id: pinch
            target: null
            onActiveChanged: if (active) {
                map.zoomLevel += pinch.scale - 1
            }
        }
        
        WheelHandler {
            id: wheel
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            rotationScale: 1/120
            property: "zoomLevel"
            target: map
        }
        
        DragHandler {
            id: drag
            target: null
            onTranslationChanged: (delta) => map.pan(-delta.x, -delta.y)
        }
    }

}
