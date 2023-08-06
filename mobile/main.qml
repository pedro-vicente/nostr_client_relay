import QtQuick
import QtQuick.Controls
import User

Window 
{
  id: id_root
  width: 750 / 2
  height: 1334 / 2
  visible: true
  title: qsTr("Mostro")

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //top bar
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Rectangle {
    id: id_topbar
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }
    height: 50
    color: "#5F8575"
    TextField {
     id: id_text_field
     placeholderText: qsTr("Pub key")
     anchors.centerIn: parent
     width: 350
     text: User.user
     onEditingFinished: {
       User.user = text
       id_list_view.visible = true
     } 
    }
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //feed
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Rectangle {
    id: id_feed
    anchors {
      top: id_topbar.bottom
      bottom: id_bottombar.top
      left: parent.left
      right: parent.right
    }
    color: "blueviolet"

    ListView {
      id: id_list_view
      model: Model

      visible: true
       
      anchors.fill: parent
      anchors.margins: 5
      clip: true
      spacing: 5
      delegate: Component {
        Rectangle {
          radius: 4
          width: id_list_view.width
          height: 200
          color: hovered ? "#f4f4f4" : "#ddd"
          border.color: "gray"

          property bool hovered: false

          Text {
            id: id_text_feed
            text: display
          }

          MouseArea {
              anchors.fill: parent
              hoverEnabled: true
              onEntered: parent.hovered = true;
              onExited: parent.hovered = false;
          }

        } //Rectangle
      } //delegate
    } //ListView
  } //Rectangle

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //bottom bar
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  Rectangle {
    id: id_bottombar
    anchors {
      bottom: parent.bottom
      left: parent.left
      right: parent.right
    }
    height: 50
    color: "#5F8575"
  }
}
