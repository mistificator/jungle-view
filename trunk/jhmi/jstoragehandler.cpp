#include "jstoragehandler.h"
#include "jstorage.h"

jStorageHandler::jStorageHandler(jStorageInterface * _storage): QObject(), storage(_storage) 
{
}

jStorageHandler::~jStorageHandler() 
{
}

void jStorageHandler::startProcessing(int _priority)	
{ 
	storage->startProcessing(_priority); 
}

void jStorageHandler::stopProcessing()											
{ 
	storage->stopProcessing(); 
}
