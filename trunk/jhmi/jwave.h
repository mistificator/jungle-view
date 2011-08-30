#ifndef __JWAVE_H__
#define __JWAVE_H__

#include "jstorage.h"

// ------------------------------------------------------------------------

class jWaveFile : public jFileStorage<qint64>
{
	COPY_FBD(jWaveFile)
public:
	jWaveFile();
	jWaveFile(const QString & _file_name);

	jFileStorage & setStorageFile(const QString & _file_name);

	jFileStorage & setOffset(quint64); // ignored
	jStorageInterface & setChannels(int); // ignored
	jStorage<qint64> & setSegmentFunc(jStorage<qint64>::segment_func); // ignored

	qint16 bits() const;
	int sampleRate() const;
protected:
	qint16 bits_per_sample;
	int sample_rate;
	static QVector< QVector<qint64> > waveSegmentProcessing(const qint64 *, quint64, jStorage<qint64> *);
};

#endif