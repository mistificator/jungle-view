#include "jwave.h"

QVector< QVector<qint64> > jWaveFile::waveSegmentProcessing(const QVector<qint64> & _items, jStorage<qint64> * _base_storage)
{
	jWaveFile * _storage = dynamic_cast<jWaveFile *>(_base_storage);
	const int _channels = _storage->channels();
	const quint64 _items_count = _items.count() * _storage->itemSize() * 8 / (_storage->bits() * _channels);
//	JDEBUG("wave seg" << _items_count);
	QVector< QVector<qint64> > _result;
	return _result;
	_result.resize(_channels);

	const qint64 * _seg_data = _items.data();

	for (int _channel = 0; _channel < _channels; _channel++)
	{
		_result[_channel].resize(_items_count);
		quint64 _offset = _channel;
		QVector<qint64> & _result_data = _result[_channel];
		switch (_storage->bits())
		{
		case 8:
			for (quint64 _idx = 0; _idx < _items_count; _idx++)
			{
				_result_data[_idx] = * (reinterpret_cast<const qint8 *>(_seg_data) + _offset);
				_offset += _channels;
			}
			break;
		case 16:
			for (quint64 _idx = 0; _idx < _items_count; _idx++)
			{
				_result_data[_idx] = * (reinterpret_cast<const qint16 *>(_seg_data) + _offset);
				_offset += _channels;
			}
			break;
		case 32:
			for (quint64 _idx = 0; _idx < _items_count; _idx++)
			{
				_result_data[_idx] = * (reinterpret_cast<const qint32 *>(_seg_data) + _offset);
				_offset += _channels;
			}
			break;
		case 64:
			for (quint64 _idx = 0; _idx < _items_count; _idx++)
			{
				_result_data[_idx] = * (_seg_data + _offset);
				_offset += _channels;
			}
			break;
		}
	}
	return _result;
}

jWaveFile::jWaveFile() : jFileStorage<qint64>()
{
	bits_per_sample = 0;
	sample_rate = 0;
	setOffset(0);
}

jWaveFile::jWaveFile(const QString & _file_name) : jFileStorage<qint64>()
{
	bits_per_sample = 0;
	sample_rate = 0;
	setStorageFile(_file_name);
}

jFileStorage<qint64> & jWaveFile::setStorageFile(const QString & _file_name)
{
	qint16 _ch_count;
	jFileStorage<qint64>::setStorageFile(_file_name);
	THREAD_SAFE(Write)
	file.seek(22);
	file.read(reinterpret_cast<char *>(& _ch_count), sizeof(_ch_count));
	file.seek(24);
	file.read(reinterpret_cast<char *>(& sample_rate), sizeof(sample_rate));
	file.seek(34);
	file.read(reinterpret_cast<char *>(& bits_per_sample), sizeof(bits_per_sample));
	THREAD_UNSAFE
	jFileStorage<qint64>::setSegmentFunc(& waveSegmentProcessing);
	jFileStorage<qint64>::setChannels(_ch_count);
	return setOffset(0);
}

jFileStorage<qint64> & jWaveFile::setOffset(quint64)
{
	SAFE_SET(offs, (quint64)44);
	return * this;
}

jStorageInterface & jWaveFile::setChannels(int)
{
	return * this;
}


qint16 jWaveFile::bits() const
{
	return bits_per_sample;
}

int jWaveFile::sampleRate() const
{
	return sample_rate;
}

QVector<qint64> jWaveFile::readItems( quint64 _items_count)
{
	THREAD_SAFE(Write)
	qint64 _file_pos = offs + position() * (bits() / 8) * channels();
	if (_file_pos < file.size())
	{
//		JDEBUG("seek" << _file_pos << "/" << file.size() << "items" << _items_count);
		file.seek(_file_pos);
		items = file.read(_items_count * (bits() / 8));
	}
	else
	{
		items.clear();
	}
	QVector<qint64> _result;
	_result.resize(items.count() / sizeof(qint64));
	::memcpy(_result.data(), items.data(), items.count());
	THREAD_UNSAFE
	return _result;
}
