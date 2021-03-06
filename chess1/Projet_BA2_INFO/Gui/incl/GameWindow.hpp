#pragma once
#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <QtWidgets>
#include <iostream>

class GameWindow : public QWidget {
    Q_OBJECT
    public:
    explicit GameWindow(QWidget *parent = nullptr);
    QPushButton *getLaunchGame() const;
    QComboBox *getComboBoxMode() const;
    QPushButton *getClassicButton() const;
    QPushButton *getDarkButton() const;
    QPushButton *getTrapistButton() const;
    QPushButton *getAntiButton() const;
    QPushButton *getExitButton() const;
    int getCurrentGameMode();

    private:
    void init_widget();
    void init_layout();
    
    QComboBox *_comboBoxMode;
    QPushButton *_classicButton;
    QPushButton *_darkButton;
    QPushButton *_trapistButton;
    QPushButton *_antiButton;
    QPushButton* _exitButton;

    QGridLayout *_gbox;
};

#endif // GAMEWINDOW_HPP
