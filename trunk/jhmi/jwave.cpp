#include "jwave.h"

QVector< QVector<qint64> > jWaveFile::waveSegmentProcessing(const qint64 * _seg_data, quint64 _seg_size, jStorage<qint64> * _base_storage)
{
	jWaveFile * _storage = dynamic_cast<jWaveFile *>(_base_storage);
	const int _channels = _storage->channels();
	const quint64 _step = _storage->bits() * _channels / 8;
	const quint64 _items_count = _seg_size /_step;
	QVector< QVector<qint64> > _result;
	_result.resize(_channels);

	for (int _channel = 0; _channel < _channels; _channel++)
	{
		_result[_channel].resize(_items_count);
		quint64 _offset = 0;
		qint64 * _result_data = const_cast<qint64 *>(_result[_channel].constData());
		switch (_storage->bits())
		{
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