#ifndef __JWAVE_H__
#define __JWAVE_H__

// ------------------------------------------------------------------------

#include "jdefs.h"
#include "jstorage.h"

class jItem;
class jStorageInterface;
class jStorageHandler;

// ------------------------------------------------------------------------

class jDataFileInterface
{
	COPY_FBD(jDataFileInterface)
    DECL_MUTEX
    DECL_PROPERTIES(jDataFileInterface)
public:
	jDataFileInterface() {}
	virtual ~jDataFileInterface() {}

	virtual jDataFileInterface & setIODevice(QIODevice * _io_device, bool _start_loading = true) = 0;

	virtual qint16 bits() const = 0;
	virtual qreal sampleRate() const = 0;
	virtual qint16 channels() const = 0;

	virtual jItem * createItem(int _channel = 0) const = 0;
	virtual jStorageInterface * storage() const = 0;
	virtual jStorageHandler * storageControl() const = 0;

	virtual qint64 item(quint64 _item_index, int _channel = 0) const = 0;

	virtual QIODevice * IODevice() const = 0;
};

// ------------------------------------------------------------------------

class jWaveFile: public jDataFileInterface
{
	PDATA
	COPY_FBD(jWaveFile)
public:
	jWaveFile();
	jWaveFile(QIODevice * _io_device, bool _start_loading = true);
	~jWaveFile();

	jDataFileInterface & setIODevice(QIODevice * _io_device, bool _start_loading = true);

	qint16 bits() const;
	qreal sampleRate() const;
	qint16 channels() const;

	jItem * createItem(int _channel = 0) const;
	jStorageInterface * storage() const;
	jStorageHandler * storageControl() const;

	qint64 item(quint64 _item_index, int _channel = 0) const;

	QIODevice * IODevice() const;
};

// ------------------------------------------------------------------------

class jFdFile: public jDataFileInterface
{
	PDATA
	COPY_FBD(jFdFile)
public:
	enum Format {No_Format = -1, Auto_Format = 0, Fd_Format = 1, Sd_Format = 2, S_Format = 3};

	jFdFile();
	jFdFile(QIODevice * _io_device, Format _fmt = Auto_Format, bool _start_loading = true);
	~jFdFile();

	jDataFileInterface & setIODevice(QIODevice * _io_device, Format _fmt = Auto_Format, bool _start_loading = true);

	qint16 bits() const;
	qreal sampleRate() const;
	qint16 channels() const;
	Format format() const;

	jItem * createItem(int _channel = 0) const;
	jStorageInterface * storage() const;
	jStorageHandler * storageControl() const;

	qint64 item(quint64 _item_index, int _channel = 0) const;

	QIODevice * IODevice() const;
};

// ------------------------------------------------------------------------

#endif
