#ifndef STYLE_H
#define STYLE_H

#define OKBTN_STYLE     "QToolButton{background-color: qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 rgb(24,196,252), stop:1 rgb(24,128,252)); border-radius:5px;color: rgb(255, 255, 255);}"  \
                        "QToolButton:hover{background-color:qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 rgb(42,215,254), stop:1 rgb(28,151,255));}"   \
                        "QToolButton:disabled{background-color:qlineargradient(spread:pad, x1:0, y1:0.5, x2:1, y2:0.5, stop:0 rgb(83,90,109), stop:1 rgb(70,76,93));color: rgb(40,45,57);}"

#define CANCELBTN_STYLE "QToolButton{background:transparent;color:rgb(192,196,212);border:1px solid rgb(70,82,113);border-radius:3px;}"  \
                        "QToolButton:hover{background-color:rgb(70,82,113);color:white;}"

#define CLOSEBTN_STYLE  "QToolButton{background-image:url(:/ui/wallet_ui/closeBtn.png);background-repeat: no-repeat;background-position: center;border-style: flat;}"   \
                        "QToolButton:hover{background-image:url(:/ui/wallet_ui/closeBtn_hover.png);"

#define CONTAINERWIDGET_STYLE   "#containerwidget{background-color:rgb(40,45,66);border-radius:10px;}"  \
                                "QLabel{color:rgb(192,196,212);}"



#endif // STYLE_H
