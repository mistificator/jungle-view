#ifndef __JWAVE_H__
#define __JWAVE_H__

// ------------------------------------------------------------------------

#include "jdefs.h"
#include "jstorage.h"

class jItem;
class jStorageInterface;
class jStorageHandler;

class jWaveFile
{
	PDATA
	COPY_FBD(jWaveFile)
	DECL_MUTEX
public:
	jWaveFile();
	jWaveFile(const QString & _file_name, bool _start_loading = true);
	~jWaveFile();

	jWaveFile & setFile(const QString & _file_name, bool _start_loading = true);

	QString fileName() const;
	qint16 bits() const;
	int sampleRate() const;
	qint16 channels() const;

	jItem * createItem(int _channel = 0) const;
	jStorageInterface * storage() const;
	jStorageHandler * storageControl() const;
};

#endif
