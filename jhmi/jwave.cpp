#include "jwave.h"
#include "jcacheditems.h"
#include "jstorage.h"

struct jWaveFile::Data
{
	qint16 bits_per_sample, ch_count;
	int sample_rate;
	QString file_name;
	Data()
	{
		bits_per_sample = 0;
		sample_rate = 0;
		ch_count = 0;
		helper = 0;
	}
	~Data()
	{
		if (helper)
		{
			delete helper;
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
		Helper(const QString & _file_name, int _channels) : HelperInterface()
		{
			strg = new jFileStorage<T, TX>(_file_name, 44);
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
			return dynamic_cast< jFileStorage<T, TX> * >(strg)->item(_item_index, _channel);
		}
	private:
		jStorageInterface * strg;
	};

	HelperInterface * helper;
};

jWaveFile::jWaveFile() : d(new Data())
{
}

jWaveFile::jWaveFile(const QString & _file_name, bool _start_loading) : d(new Data())
{
	setFile(_file_name, _start_loading);
}

jWaveFile::~jWaveFile()
{
	delete d;
}

jWaveFile & jWaveFile::setFile(const QString & _file_name, bool _start_loading)
{
	THREAD_SAFE(Write)
	if (d->helper)
	{
		delete d->helper;
	}
	QFile _file(d->file_name = _file_name);
	_file.setPermissions(QFile::ReadOther);
	_file.open(QFile::ReadOnly);
	_file.seek(22);
	_file.read(reinterpret_cast<char *>(& d->ch_count), sizeof(d->ch_count));
	_file.seek(24);
	_file.read(reinterpret_cast<char *>(& d->sample_rate), sizeof(d->sample_rate));
	_file.seek(34);
	_file.read(reinterpret_cast<char *>(& d->bits_per_sample), sizeof(d->bits_per_sample));
	_file.close();
	switch (d->bits_per_sample)
	{
	case 8:
		d->helper = new Data::Helper<qint8, qint64>(_file_name, d->ch_count);
		break;
	case 16:
		d->helper = new Data::Helper<qint16, qint64>(_file_name, d->ch_count);
		break;
	case 32:
		d->helper = new Data::Helper<qint32, qint64>(_file_name, d->ch_count);
		break;
	case 64:
		d->helper = new Data::Helper<qint64, qint64>(_file_name, d->ch_count);
		break;
	}
	if (_start_loading)
	{
		d->helper->storage()->startProcessing();
	}
	THREAD_UNSAFE
	return * this;
}

qint16 jWaveFile::bits() const
{
	return d->bits_per_sample;
}

int jWaveFile::sampleRate() const
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
	return SAFE_GET(d->helper->createItem(_channel));
}

jStorageInterface * jWaveFile::storage() const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return SAFE_GET(d->helper->storage());
}

jStorageHandler * jWaveFile::storageControl() const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return SAFE_GET(d->helper->storage()->storageControl());
}

QString jWaveFile::fileName() const
{
	return d->file_name;
}

qint64 jWaveFile::item(quint64 _item_index, int _channel) const
{
	if (d->helper == 0)
	{
		return 0;
	}
	return d->helper->item(_item_index, _channel);
}
