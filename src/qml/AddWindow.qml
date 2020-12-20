import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3

import Style 1.0

Dialog {
    id: _dialog

    property alias country: _country

    x: (root.width - width) / 2
    y: (root.height - height) / 2
    width: root.width - Style.mediumOffset * 2
    modal: true

    standardButtons: Dialog.Save | Dialog.Cancel
    closePolicy: Popup.NoAutoClose
    Material.theme: Material.Dark
    Material.accent: Material.Indigo
    Material.primary: Material.BlueGrey

    margins: Style.lowOffset

    function clear() {
        _name.text = ""
        _telephone.text = ""
        _country.currentIndex = 0
    }

    function serialize() {
        var text = "{"
        text += "\"_id\":0,"
        text += "\"name\":\"" + _name.text + "\","
        text += "\"telephone\":\"" + _telephone.text + "\","
        text += "\"country\":" + _country.currentId()

        return text + "}"
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: Style.lowSpacing

        Label {
            text: qsTr("New Person:")
            font.bold: true
            font.pointSize: 14
        }

        TextField {
            id: _name

            Layout.fillWidth: true

            focus: true
            placeholderText: qsTr("Input name")
            font.pointSize: 12
            selectByMouse: true
            activeFocusOnPress: true
            maximumLength: 30
            validator: RegExpValidator { regExp: /(\w+\s?)+/ }
        }

        TextField {
            id: _telephone

            Layout.fillWidth: true

            focus: true
            placeholderText: qsTr("Input telephone")
            font.pointSize: 12
            selectByMouse: true
            activeFocusOnPress: true
            maximumLength: 20
            validator: RegExpValidator { regExp: /[1-9]{1,1}\d+/ }
        }

        RowLayout {
            spacing: Style.lowSpacing

            Label { text: qsTr("Country") + ":" }

            ComboBox {
                id: _country

                function currentId() {
                    return countryModel.get(_country.currentIndex).value;
                }

                function id(text) {
                    for(var i=0; i<countryModel.count; ++i) {
                        var obj = countryModel.get(i)
                        if(obj.key === text)
                            return obj.value
                    }

                    return -1;
                }

                function text(id) {
                    for(var i=0; i<countryModel.count; ++i) {
                        var obj = countryModel.get(i)
                        if(obj.value === id)
                            return obj.key
                    }

                    return ""
                }

                textRole: "key"

                model: ListModel {
                    id: countryModel

                    ListElement { key: "Russia";    value: 8 }
                    ListElement { key: "USA";       value: 1 }
                    ListElement { key: "Ukraine";   value: 38 }
                }

                popup: Popup {
                    y: _country.height - 1
                    width: _country.width
                    implicitHeight: contentItem.implicitHeight
                    padding: 1

                    contentItem: ListView {
                        Material.theme: Material.Light

                        clip: true
                        implicitHeight: contentHeight
                        model: _country.popup.visible ? _country.delegateModel : null
                        currentIndex: _country.highlightedIndex

                        ScrollIndicator.vertical: ScrollIndicator { }
                    }
                }
            }
        }
    }

    onAccepted: {
        if(_name.length < 1 || _telephone.length < 1)
            console.log("The name and phone fillds are to be filled")
        else
            client.add(serialize())

        clear()
    }

    onRejected: {
        clear()
    }
}
