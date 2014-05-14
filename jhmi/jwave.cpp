#include "jwave.h"
#include "jcacheditems.h"
#include "jstorage.h"

// ------------------------------------------------------------------------

struct jWaveFile::Data
{
	qint16 bits_per_sample, ch_count;
	qint32 sample_rate;
	QIODevice * io_device;
	Data()
	{
		bits_per_sample = 0;
		sample_rate = 0;
		ch_count = 0;
		helper = 0;
		io_device = 0;
	}
	~Data()
	{
		if (helper)
		{
			delete helper;
		}
		if (io_device)
		{
			if (io_device->isOpen())
			{
				io_device->close();
			}
		}
	}

	class HelperInterface
	{
	public:
		HelperInterface() {}
		virtual ~HelperInterface() {}
		virtual jItem * createItem(int _channel) const = 0;
		virtual jStorageInterface * storage() const = 0;
		virtual qint64 item(quint64 _item_index, int _channel) const = 0;
	};

	template <class T, class TX>
	class Helper : public HelperInterface
	{
	public:
		Helper(QIODevice * _io_device, int _channels) : HelperInterface()
		{
			strg = new jIODeviceStorage<T, TX>(_io_device, 44);
			strg->setChannels(_channels);
		}
		~Helper()
		{
			delete strg;
		}
		jItem * createItem(int _channel) const
		{
			if ((_channel < 0) || (_channel >= strg->channels()))
			{
				return 0;
			}
			return new jCachedItem1D<T, TX>(strg, _channel, jCachedItem1D<T, TX>::Lines);
		}
		jStorageInterface * storage() const
		{
			return strg;
		}
		qint64 item(quint64 _item_index, int _channel) const
		{
			return dynamic_cast< jIODeviceStorage<T, TX> * >(strg)->item(_item_index, _channel);
		}
	private:
		jStorageInterface * strg;
	};

	HelperInterface * helper;
};

jWaveFile::jWaveFile() : jDataFileInterface(), d(new Data())
{
}

jWaveFile::jWaveFile(QIODevice * _io_device, bool _start_loading) : jDataFileInterface(), d(new Data())
{
	setIODevice(_io_device, _start_loading);
}

jWaveFile::~jWaveFile()
{
	delete d;
}

jDataFileInterface & jWaveFile::setIODevice(QIODevice * _io_device, bool _start_loading)
{
	if (d->helper)
	{
		delete d->helper;
	}
	if (d->io_device)
	{
		if (d->io_device->isOpen())
		{
			d->io_device->close();
		}
	}
	d->io_device = _io_device;
	if (d->io_device == 0)
	{
		return * this;
	}
	d->io_device->open(QIODevice::ReadOnly);
	d->io_device->seek(22);
	d->io_device->read(reinterpret_cast<char *>(& d->ch_count), sizeof(d->ch_count));
	d->io_device->seek(24);
	d->io_device->read(reinterpret_cast<char *>(& d->sample_rate), sizeof(d->sample_rate));
	d->io_device->seek(34);
	d->io_device->read(reinterpret_cast<char *>(& d->bits_per_sample), sizeof(d->bits_per_sample));
	d->io_device->close();
	switch (d->bits_per_sample)
	{
	case 8:
		d->helper = new Data::Helper<qint8, qint64>(d->io_device, d->ch_count);
		break;
	case 16:
		d->helper = new Data::Helper<qint16, qint64>(d->io_device, d->ch_count);
		break;
	case 32:
		d->helper = new Data::Helper<qint32, qint64>(d->io_device, d->ch_count);
		break;
	case 64:
		d->helper = new Data::Helper<qint64, qint64>(d->io_device, d->ch_count);
		break;
	}
	if (_start_loading)
	{
		d->helper->storage()->startProcessing();
	}
	return * this;
}


qint16 jWaveFile::bits() const
{
	return d->bits_per_sample;
}

qreal jWaveFile::sampleRate() const
{
	return d->sample_rate;
}

qint16 jWaveFile::channels() const
{
	return d->ch_count;
}

jItem * jWaveFile::createItem(int _channel) const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->createItem(_channel);
}

jStorageInterface * jWaveFile::storage() const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->storage();
}

jStorageHandler * jWaveFile::storageControl() const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->storage()->storageControl();
}

qint64 jWaveFile::item(quint64 _item_index, int _channel) const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->item(_item_index, _channel);
}

QIODevice * jWaveFile::IODevice() const
{
	return d->io_device;
}

// ------------------------------------------------------------------------

struct jFdFile::Data
{
	qint16 bits_per_sample;
	jFdFile::Format set_format, actual_format;
	QIODevice * io_device;
	qreal sample_rate;
	Data()
	{
		bits_per_sample = 0;
		sample_rate = 0;
		helper = 0;
		io_device = 0;
		set_format = jFdFile::Auto_Format;
		actual_format = jFdFile::No_Format;
	}
	~Data()
	{
		if (helper)
		{
			delete helper;
		}
		if (io_device)
		{
			if (io_device->isOpen())
			{
				io_device->close();
			}
		}
	}

	class HelperInterface
	{
	public:
		HelperInterface() {}
		virtual ~HelperInterface() {}
		virtual jItem * createItem(int _channel) const = 0;
		virtual jStorageInterface * storage() const = 0;
		virtual qint64 item(quint64 _item_index, int _channel) const = 0;
	};

	template <class T, class TX>
	class Helper : public HelperInterface
	{
	public:
		Helper(QIODevice * _io_device, int _offset) : HelperInterface()
		{
			strg = new jIODeviceStorage<T, TX>(_io_device, _offset);
			strg->setChannels(2);
		}
		~Helper()
		{
			delete strg;
		}
		jItem * createItem(int _channel) const
		{
			if ((_channel < 0) || (_channel >= strg->channels()))
			{
				return 0;
			}
			return new jCachedItem1D<T, TX>(strg, _channel, jCachedItem1D<T, TX>::Lines);
		}
		jStorageInterface * storage() const
		{
			return strg;
		}
		qint64 item(quint64 _item_index, int _channel) const
		{
			return dynamic_cast< jIODeviceStorage<T, TX> * >(strg)->item(_item_index, _channel);
		}
	private:
		jStorageInterface * strg;
	};

	HelperInterface * helper;
};

jFdFile::jFdFile() : jDataFileInterface(), d(new Data())
{
}

jFdFile::jFdFile(QIODevice * _io_device, Format _fmt, bool _start_loading) : jDataFileInterface(), d(new Data())
{
	setIODevice(_io_device, _fmt, _start_loading);
}

jFdFile::~jFdFile()
{
	delete d;
}

jDataFileInterface & jFdFile::setIODevice(QIODevice * _io_device, Format _fmt, bool _start_loading)
{
	if (d->helper)
	{
		delete d->helper;
	}
	if (d->io_device)
	{
		if (d->io_device->isOpen())
		{
			d->io_device->close();
		}
	}
	d->io_device = _io_device;
	d->sample_rate = 0;
	d->bits_per_sample = 0;
	d->actual_format = No_Format;
	if (d->io_device == 0)
	{
		return * this;
	}
	d->set_format = _fmt;
	if (d->set_format == Auto_Format)
	{
		QFile * _file = dynamic_cast<QFile *>(_io_device);
		if (_file)
		{
			if (QFileInfo(* _file).suffix().compare("fd", Qt::CaseInsensitive) == 0)
			{
				d->actual_format = Fd_Format;
			}
			else
			if (QFileInfo(* _file).suffix().compare("sd", Qt::CaseInsensitive) == 0)
			{
				d->actual_format = Sd_Format;
			}
			else
			if (QFileInfo(* _file).suffix().compare("s", Qt::CaseInsensitive) == 0)
			{
				d->actual_format = S_Format;
			}
		}
	}
	else
	{
		d->actual_format = d->set_format;
	}
	switch (d->actual_format)
	{
	case Fd_Format:
	case Sd_Format:
		d->io_device->open(QIODevice::ReadOnly);
		d->io_device->seek(0);
		d->io_device->read(reinterpret_cast<char *>(& d->sample_rate), sizeof(d->sample_rate));
		d->io_device->close();
		d->sample_rate = d->sample_rate * 1000.0;
		break;
	case S_Format:
		d->sample_rate = 5000;
		break;
	}
	switch (d->actual_format)
	{
	case Fd_Format:
		d->bits_per_sample = 32;
		d->helper = new Data::Helper<qreal, qint64>(d->io_device, sizeof(d->sample_rate));
		break;
	case Sd_Format:
		d->bits_per_sample = 16;
		d->helper = new Data::Helper<qint16, qint64>(d->io_device, sizeof(d->sample_rate));
		break;
	case S_Format:
		d->bits_per_sample = 16;
		d->helper = new Data::Helper<qint16, qint64>(d->io_device, 0);
		break;
	}
	if (_start_loading)
	{
		d->helper->storage()->startProcessing();
	}
	return * this;
}

qint16 jFdFile::bits() const
{
	return d->bits_per_sample;
}

qreal jFdFile::sampleRate() const
{
	return d->sample_rate;
}

qint16 jFdFile::channels() const
{
	return 2;
}

jFdFile::Format jFdFile::format() const
{
	return d->actual_format;
}

jItem * jFdFile::createItem(int _channel) const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->createItem(_channel);
}

jStorageInterface * jFdFile::storage() const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->storage();
}

jStorageHandler * jFdFile::storageControl() const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->storage()->storageControl();
}

qint64 jFdFile::item(quint64 _item_index, int _channel) const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->item(_item_index, _channel);
}

QIODevice * jFdFile::IODevice() const
{
	return d->io_device;
}

// ------------------------------------------------------------------------
