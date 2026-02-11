import QtQuick
import QtLocation
import QtPositioning

Rectangle    {
    id:window

    property double latitude: -31.756285
    property double longitude: -52.335829

    property Component locationmarker: marker
    
    function setPath(pathCoords) {
        routeLine.path = pathCoords;
    }

    function setMarkers(startCoord, endCoord) {
        startMarker.coordinate = startCoord;
        endMarker.coordinate = endCoord;
        startMarker.visible = true;
        endMarker.visible = true;
    }

    Plugin
    {
        id:openstreetview
        name:"osm"
        PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: "true" }
        PluginParameter { name: "osm.mapping.highdpi_tiles"; value: "true" }
    }
    Map{
        id:map
        anchors.fill: parent
        plugin: openstreetview
        center: QtPositioning.coordinate(latitude, longitude)
        zoomLevel: 14
        
        MapPolyline {
            id: routeLine
            line.width: 5
            line.color: "blue"
        }

        MapQuickItem {
            id: startMarker
            visible: false
            anchorPoint.x: sourceItem.width / 2
            anchorPoint.y: sourceItem.height
            sourceItem: Rectangle {
                width: 20
                height: 20
                color: "green"
                radius: 10
                border.color: "white"
                border.width: 2
            }
        }

        MapQuickItem {
            id: endMarker
            visible: false
            anchorPoint.x: sourceItem.width / 2
            anchorPoint.y: sourceItem.height
            sourceItem: Rectangle {
                width: 20
                height: 20
                color: "red"
                radius: 10
                border.color: "white"
                border.width: 2
            }
        }
        
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
