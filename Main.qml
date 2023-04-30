import QtQuick
import QtQuick.Window
import Contacts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Android Contacts App")

    ContactsModel{
        id:contactsModel
    }

    ListView{
        anchors{fill: parent; leftMargin:20}
        clip:true
        boundsBehavior: Flickable.StopAtBounds
        spacing:10
        model:contactsModel
        delegate: Column{
            Text{text:model.name; font.pointSize:25}
            Text{text:model.phoneNumber; font.pointSize:25}
        }
    }
}
