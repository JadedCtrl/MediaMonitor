/*
 * Copyright 2022, Jaidyn Levesque <jadedctrl@teknik.io>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef _APP_H
#define _APP_H

#include <Application.h>


class App: public BApplication
{
public:
	              App();

	virtual void  AboutRequested();
};


int main(int argc, char** argv);

#endif // _APP_H
