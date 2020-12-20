import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as Cntrl1
import QtQuick.Layouts 1.3

import Style 1.0

ApplicationWindow {
    id: root

    visible: true
    width: 640
    height: 480
    title: qsTr("Task STC")

    property bool connected: false

    function findPerson(id) {
        for(var i=0; i<personModel.count; ++i) {
            if(personModel.get(i)._id === id)
                return i
        }

        return -1
    }

    Connections {
        target: client
        onAddPerson: {
            var obj = JSON.parse(jData)
            var index = findPerson(obj._id)

            obj.country = addWindow.country.text(obj.country)

            if(index < 0)
                personModel.append(obj)
            else
                personModel.set(index, obj)
        }

        onRemovePerson: {
            var obj = JSON.parse(jData)
            var index = findPerson(obj._id)

            if(index < 0)
                return

            personModel.remove(index)
        }

        onConnected: {
            connected = state
        }
    }

    ListModel {
        id: personModel
    }

    Cntrl1.TableView {
        id: _tableView

        width: root.width
        height: root.height

        Cntrl1.TableViewColumn {
            role: "_id"
            title: "ID"
            width: 30
        }
        Cntrl1.TableViewColumn {
            role: "name"
            title: "Name"
            width: 200
        }
        Cntrl1.TableViewColumn {
            role: "telephone"
            title: "Phone"
            width: 100
        }
        Cntrl1.TableViewColumn {
            role: "country"
            title: "Country"
            width: 100
        }
        model: personModel

        backgroundVisible: false

        headerDelegate: Rectangle{
            height: 30
            border.width: 1
            color: "#333"
            border.color: "#555"

            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: styleData.value
                font.pointSize: 11
                font.bold: true
                color: Style.textColor
            }
        }

        itemDelegate: Item {
            Text {
                anchors.fill: parent
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                text: styleData.value
                font.pointSize: 10
                color: Style.textColor
            }
        }

        rowDelegate: Rectangle {
            color: styleData.selected ? "#001e7e" : (styleData.alternate ? "#666" : "#777")
            height: 24

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.RightButton

                onClicked: {
                    if(_tableView.rowCount > styleData.row) {
                        _tableView.selection.clear()
                        _tableView.selection.select(styleData.row)
                        _tableView.currentRow = styleData.row
                    }

                    if(mouse.button == Qt.RightButton)
                        _contextMenu.popup()
                }

                Menu {
                    id: _contextMenu

                    Action {
                        text: qsTr("New")
                        onTriggered: addWindow.open()
                    }

                    Action {
                        function serialize(obj) {
                            var text = "{"
                            text += "\"_id\":" + obj._id + ","
                            text += "\"name\":\"" + obj.name + "\","
                            text += "\"telephone\":\"" + obj.telephone + "\","
                            text += "\"country\":" + addWindow.country.id(obj.country)

                            return text + "}"
                        }

                        text: qsTr("Delete")
                        onTriggered: {
                            if(_tableView.currentRow < 0)
                                return

                            var obj = personModel.get(_tableView.currentRow)
                            client.remove(serialize(obj))
                            _tableView.selection.clear()
                            _tableView.currentRow = -1
                        }
                    }

                    delegate: MenuItem {
                        id: _menuItem
                        implicitWidth: 200
                        implicitHeight: 40

                        contentItem: Text {
                            leftPadding: _menuItem.indicator.width
                            rightPadding: _menuItem.arrow.width
                            text: _menuItem.text
                            font: _menuItem.font
                            opacity: enabled ? 1.0 : 0.3
                            color: _menuItem.highlighted ? "#fff" : Style.textColor
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }

                    background: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 40
                        color: "#444"
                        border.color: "#666"
                        radius: 2
                    }
                }

            }
        }
    }

    AddWindow {
        id: addWindow

        onClosed: {
            _tableView.selection.clear()
            _tableView.currentRow = -1
        }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: qsTr("Export")
                visible: isServer
                onClicked: client.exportFile()
            }

            Label {
                visible: isServer
                text: qsTr("Server")
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            Label {
                visible: !isServer
                text: qsTr("Connected")
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
                color: connected ? "#00ee00" : "#ee0000"
            }
        }
    }
}
