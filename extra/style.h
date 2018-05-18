#ifndef STYLE_H
#define STYLE_H

#define OKBTN_STYLE     "QToolButton{font: 14px \"微软雅黑\";background-color: rgb(84,116,235);border:0px solid white;border-radius:15px;color: white;}"  \
                        "QToolButton:hover{background-color:rgb(0,210,255);}"   \
                        "QToolButton:disabled{background-color:rgb(235,235,235);}"

#define CANCELBTN_STYLE "QToolButton{background:rgb(229,229,229);color: white;border:0px solid white;border-radius:15px;}"  \
                        "QToolButton:hover{background-color:rgb(0,210,255);}"

#define CLOSEBTN_STYLE  "QToolButton{background-image:url(:/ui/wallet_ui/close.png);background-repeat: no-repeat;background-position: center;background-color:transparent;border:none;}"   \
                        "QToolButton:hover{background-color:rgb(208,228,255);"

#define CONTAINERWIDGET_STYLE   "#containerwidget{background-color:rgb(255,255,255);border-radius:10px;}"  \
                                "QLabel{color:rgb(51,51,51);}"

#define TOOLBUTTON_STYLE_1      "QToolButton{font: 11px \"微软雅黑\";background-color:#00D2FF; border:none;border-radius:10px;color: rgb(255, 255, 255);}" \
                                "QToolButton:hover{background-color:#5474EB;}"

#define TABLEWIDGET_STYLE_1     "QTableView{background-color:white;border:1px solid rgb(236,238,251);border-radius:15px;font: 12px \"微软雅黑\";}" \
                                "QHeaderView{border:none;border-top-left-radius:15px;border-top-right-radius:15px;background-color:white;color:#C6CAD4;font: 12px \"微软雅黑\";}" \
                                "QHeaderView:section{height:35px;border:none;background-color:white;}" \
                                "QHeaderView:section:first{border-top-left-radius:15px;}" \
                                "QHeaderView:section:last{border-top-right-radius:15px;}"

#define SELECTBTN_STYLE "QToolButton{background:transparent;color:rgb(192,202,212);font: 11px \"微软雅黑\";}" \
                        "QToolButton:hover{color:rgb(84,116,235);}"

#define COMBOBOX_STYLE_BOTTOMBORDER "QComboBox{background-color:transparent;border-top:none;border-left:none;border-right:none;border-bottom:1px solid gray;color: black;padding-left: 5px;font: 14px \"微软雅黑\";}" \
                                    "QComboBox::drop-down {background:transparent;border:none;}" \
                                    "QComboBox::down-arrow {image: url(:/ui/wallet_ui/downArrow.png);}" \
                                    "QComboBox::down-arrow:on {image: url(:/ui/wallet_ui/downArrow_on.png);}" \
                                    "QComboBox QAbstractItemView {outline: 0px;}"

#endif // STYLE_H
