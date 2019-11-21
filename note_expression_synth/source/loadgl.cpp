#include "loadgl.h"

Loadgl* Loadgl::myInstance = NULL;

Loadgl* Loadgl::Instance() {
	if (!myInstance) {
		myInstance = new Loadgl;
		myInstance->init();
	}
	return myInstance;
}