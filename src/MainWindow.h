/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Window.h>


class MainWindow : public BWindow {
public:
	MainWindow();

private:
	void _InitInterface();
};

#endif // MAINWINDOW_H
