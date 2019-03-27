######################################################################
# Automatically generated by qmake (3.1) Mon Mar 11 21:22:15 2019
######################################################################

QT += widgets
QMAKE_CXXFLAGS += -std=c++11 -g


TEMPLATE = app
TARGET = cli

#
OBJECTS_DIR = Gui/bin
MOC_DIR = Gui/bin

# path
INCLUDEPATH += ./Gui/src
INCLUDEPATH += ./Gui/incl
INCLUDEPATH += ./Gui/img
INCLUDEPATH += ./Gui/bin
INCLUDEPATH += ./Gui/advertisements
INCLUDEPATH += ./Gui
INCLUDEPATH += .

###
INCLUDEPATH += ./Client
INCLUDEPATH += ./Client/includes
INCLUDEPATH += ./Client/src
INCLUDEPATH += ./Client/game/common/MyException
INCLUDEPATH += ./Client/game/common/MyException/MyOstream
INCLUDEPATH += ./Client/game/GameType/SuperGame
###

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNIClient/game/GameType/SuperGame/NGS
#Client/game/GameType/SuperGame/
# You can also make your code Client/game/GameType/SuperGame/fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += Gui/incl/FenPrincipale.hpp \
           Gui/incl/GameWindow.hpp \
           Gui/incl/Login.hpp \
           Gui/incl/PlateauBox.hpp \
           Gui/incl/PlateauScene.hpp \
           Gui/incl/Register.hpp \
           Gui/incl/Menu.hpp\
           Gui/incl/Chat.hpp \
           Gui/incl/ChessItem.hpp\
           Gui/incl/Timer.hpp\
           Gui/incl/FriendList.hpp\
           Gui/incl/Publicity.hpp\
           ###
           Client/includes/abstractClient.hpp\
           Client/includes/abstractInterface.hpp\
           Client/game/GameTypes/SuperGame/abstractPlayer.hpp\
           Client/includes/client.hpp\
           Client/includes/request.hpp\
           Client/includes/terminal.hpp\
           Client/game/common/MyException/MyOstream/MyOstream.hpp \
           Client/game/common/MyException/MyException.hpp

SOURCES += Gui/src/FenPrincipale.cpp \
           Gui/src/GameWindow.cpp \
           Gui/src/Login.cpp \
           #Gui/src/main.cpp \
           Gui/src/PlateauBox.cpp \
           Gui/src/PlateauScene.cpp \
           Gui/src/Register.cpp\
           Gui/src/Menu.cpp \
           Gui/src/Chat.cpp \
           Gui/src/ChessItem.cpp \
           Gui/src/Timer.cpp \
           Gui/src/FriendList.cpp\
           Gui/src/Publicity.cpp\
           ###
           Client/main.cpp\
           Client/srcs/client.cpp \
           Client/srcs/request.cpp \
           Client/srcs/terminal.cpp\
           
