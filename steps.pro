VERSION = 0.1.7
TARGET = Steps

# Qt packages to use
CONFIG += mobility
CONFIG += qt-components
MOBILITY += sensors
MOBILITY += multimedia
QT += sql
QT += network
QT += script

# Meego Harmattan rules
contains(MEEGO_EDITION,harmattan) {
    CONFIG += link_pkgconfig qmsystem2
    PKGCONFIG += libresourceqt1
    RESOURCES += meego.qrc
    SOURCES += mediakey.cpp
    HEADERS += mediakey.h
    DATADIR = share
    DEFINES += STEPS_DATADIR=\\\"/opt/Steps/share\\\"
    DEFINES += STEPS_VERSION=\\\"$$VERSION\\\"

    # Add a splash image for the Meego launcher
    folder_splash.source = splash
    folder_splash.target = $$DATADIR
    DEPLOYMENTFOLDERS += folder_splash
}

# Symbian rules
symbian {
    TARGET.CAPABILITY += NetworkServices ReadDeviceData WriteDeviceData
    LIBS += -L\\epoc32\\release\\armv5\\lib -lsysutil -lpsmclient -lws32 -lapgrfx -lavkon
    RESOURCES += symbian.qrc
    DATADIR = c:/data/Steps
    DEFINES += STEPS_DATADIR='"c:/data/Steps"'
    DEFINES += STEPS_VERSION='"$$VERSION"'

    # For Nokia Store
    vendorinfo += "%{\"pipacs\"}" ":\"pipacs\""
    my_deployment.pkg_prerules += vendorinfo
    DEPLOYMENT += my_deployment

    # Use official UID for Steps
    TARGET.UID3 += 0x20034d0f
    # Use official UID for wrapper package
    # DEPLOYMENT.installer_header = 0x2002CCCF
}

# Add "sounds" folder to the application
# (Sounds cannot be played back from resource on Symbian)
folder_sounds.source = sounds
folder_sounds.target = $$DATADIR
DEPLOYMENTFOLDERS += folder_sounds

SOURCES += \
    main.cpp \
    preferences.cpp \
    platform.cpp \
    logger.cpp \
    sipfixer.cpp \
    uploader.cpp \
    gftprogram.cpp \
    gft.cpp \
    o2/o2.cpp \
    o2/o2replyserver.cpp \
    o2/simplecrypt.cpp \
    database.cpp \
    trace.cpp \
    detector.cpp \
    o2/o2requestor.cpp \
    o2/o2reply.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    README.txt \
    qml/symbian/Beep.qml \
    sounds/start.wav \
    sounds/beep.wav \
    sounds/stop.wav \
    background.svg \
    qml/Beep.qml \
    qml/main.qml \
    qml/Dummy.qml \
    sounds/settings.wav \
    qml/MainPage.qml \
    qml/meego/StepsLabel.qml \
    qml/meego/StepsPage.qml \
    qml/meego/StepsPageStackWindow.qml \
    qml/meego/StepsCheckBox.qml \
    qml/meego/StepsSlider.qml \
    qml/meego/StepsDialog.qml \
    qml/SettingsPage.qml \
    qml/symbian/StepsPageStackWindow.qml \
    qml/symbian/StepsPage.qml \
    qml/symbian/StepsLabel.qml \
    qml/symbian/StepsCheckBox.qml \
    qml/symbian/StepsSlider.qml \
    qml/symbian/StepsDialog.qml \
    steps256.png \
    qml/ActionsPage.qml \
    qml/meego/StepsButton.qml \
    qml/BigButton.qml \
    qml/symbian/StepsButton.qml \
    qml/symbian/StepsYesNoDialog.qml \
    images/steps.png \
    qml/meego/StepsYesNoDialog.qml \
    images/settings.png \
    images/reset.png \
    qml/meego/StepsTextField.qml \
    qml/LoginBrowser.qml \
    qml/meego/StepsSpinner.qml \
    qml/symbian/StepsSpinner.qml \
    qml/meego/StepsBanner.qml \
    qml/symbian/StepsBanner.qml \
    publishing/steps256.png \
    publishing/screenshots/meego/2011-12-17_19-28-06.png \
    publishing/screenshots/meego/2011-12-17_19-27-39.png \
    publishing/screenshots/meego/2011-12-17_19-26-44.png \
    publishing/screenshots/meego/2011-12-17_19-26-37.png \
    publishing/screenshots/meego/2011-12-17_19-26-21.png \
    publishing/screenshots/meego/2011-12-17_19-28-32.png \
    publishing/screenshots/symbian/symbian-3.png \
    publishing/screenshots/symbian/symbian-2.png \
    publishing/screenshots/symbian/symbian-1.png \
    publishing/screenshots/symbian/symbian-4.png \
    qml/meego/StepsRedButton.qml \
    qml/symbian/StepsRedButton.qml \
    qml/BigRedButton.qml \
    publishing/splash-n9.pxm \
    splash/splash-n9.jpg \
    publishing/splash-n9.png \
    qml/symbian/StepsButtonColumn.qml \
    qml/meego/StepsButtonColumn.qml \
    qml/Splash.qml \
    images/splash.jpg \
    qml/symbian/StepsTextField.qml \
    psm-symbian/install.bat \
    qml/meego/StepsToolBarLayout.qml \
    qml/meego/StepsToolIcon.qml \
    qml/symbian/StepsToolIcon.qml \
    qml/symbian/StepsToolBarLayout.qml \
    qml/AboutPage.qml \
    qml/meego/StepsScrollDecorator.qml \
    qml/symbian/StepsScrollDecorator.qml \
    texts/about.html \
    texts/hu/about.html \
    images/about.png \
    qml/meego/StepsToolButton.qml \
    qml/symbian/StepsToolButton.qml \
    images/play.png \
    images/pause.png \
    qml/symbian/StepsFlickable.qml \
    qml/meego/StepsFlickable.qml \
    translations/zh_CN.ts \
    translations/hu.ts \
    translations/da.ts \
    translations/it.ts \
    qml/TweaksPage.qml \
    qml/Gap.qml

HEADERS += \
    preferences.h \
    platform.h \
    logger.h \
    sipfixer.h \
    uploader.h \
    gftprogram.h \
    gft.h \
    o2/o2.h \
    o2/o2replyserver.h \
    o2/simplecrypt.h \
    gftsecret.h \
    database.h \
    trace.h \
    detector.h \
    o2/o2requestor.h \
    o2/o2reply.h

RESOURCES += \
    common.qrc \
    translations.qrc
