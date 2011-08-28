#include "jstoragehandler.h"
#include "jstorage.h"

jStorageHandler::jStorageHandler(jStorageInterface * _storage): QObject(), storage(_storage) 
{
}

jStorageHandler::~jStorageHandler() 
{
}

void jStorageHandler::emitFinished(quint64 _msecs)	
{ 
	emit finished(_msecs); 
}

void jStorageHandler::emitLayersAdjusted()			
{ 
	emit layersAdjusted(); 
}

void jStorageHandler::emitStopped()			
{ 
	emit stopped(); 
}

void jStorageHandler::startProcessing(int _priority)	
{ 
	storage->startProcessing(_priority); 
}

void jStorageHandler::stopProcessing()											
{ 
	storage->stopProcessing(); 
}
