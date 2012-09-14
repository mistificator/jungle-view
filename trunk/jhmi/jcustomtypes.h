#ifndef _JCUSTOMTYPES_H_
#define _JCUSTOMTYPES_H_

template <class T, quint32 Size> // 65536 max
struct jCustomType
{
	jCustomType()
	{
		::qMemSet(data, 0, sizeof(data));
	}
	jCustomType(const T * _data)
	{
		if (_data != data)
		{
			::qMemCopy(data, _data, sizeof(data));
		}
	}
	jCustomType(const jCustomType & _other)
	{
		::qMemCopy(data, _other.data, sizeof(data));
	}
	jCustomType & operator = (const jCustomType & _other)
	{
		if (&_other != this)
		{
			::qMemCopy(data, _other.data, sizeof(data));
		}
		return * this;
	}
	bool operator < (const jCustomType & _other) const
	{
		quint16 _conditional = 0;
		for (quint16 _idx = 0; _idx < Size; _idx++)
		{
			_conditional += (data[_idx] < _other.data[_idx]);
		}
		return (_conditional > (Size / 2));
	}
	bool operator > (const jCustomType & _other) const
	{
		quint16 _conditional = 0;
		for (quint16 _idx = 0; _idx < Size; _idx++)
		{
			_conditional += (data[_idx] > _other.data[_idx]);
		}
		return (_conditional > (Size / 2));
	}
	operator T * & ()
	{
		return data;
	}
	operator T * & () const
	{
		return data;
	}

	T data[Size];
};

template <class T, quint32 Size>
QDataStream& operator<<(QDataStream& s, const QVector< jCustomType<T, Size> >& v)
{
    s << quint32(v.size());
    for (QVector< jCustomType<T, Size> >::const_iterator it = v.begin(); it != v.end(); ++it)
        s << QByteArray((char *)it->data, sizeof(it->data));
    return s;
}

template <class T, quint32 Size>
QDataStream& operator>>(QDataStream& s, QVector< jCustomType<T, Size> >& v)
{
    v.clear();
    quint32 c;
    s >> c;
    v.resize(c);
    for(quint32 i = 0; i < c; ++i) {
        QByteArray t;
        s >> t;
		::qMemCopy(v[i].data, t.data(), t.count());
    }
    return s;
}

typedef jCustomType<qint8, 1>		I8Size1;
typedef jCustomType<qint8, 2>		I8Size2;
typedef jCustomType<qint8, 4>		I8Size4;
typedef jCustomType<qint8, 8>		I8Size8;
typedef jCustomType<qint8, 16>		I8Size16;
typedef jCustomType<qint8, 32>		I8Size32;
typedef jCustomType<qint8, 64>		I8Size64;
typedef jCustomType<qint8, 128>		I8Size128;
typedef jCustomType<qint8, 256>		I8Size256;
typedef jCustomType<qint8, 512>		I8Size512;
typedef jCustomType<qint8, 1024>	I8Size1024;
typedef jCustomType<qint8, 2048>	I8Size2048;
typedef jCustomType<qint8, 4096>	I8Size4096;
typedef jCustomType<qint8, 8192>	I8Size8192;
typedef jCustomType<qint8, 16384>	I8Size16384;
typedef jCustomType<qint8, 32768>	I8Size32768;
typedef jCustomType<qint8, 65536>	I8Size65536;

typedef jCustomType<qint16, 1>		I16Size1;
typedef jCustomType<qint16, 2>		I16Size2;
typedef jCustomType<qint16, 4>		I16Size4;
typedef jCustomType<qint16, 8>		I16Size8;
typedef jCustomType<qint16, 16>		I16Size16;
typedef jCustomType<qint16, 32>		I16Size32;
typedef jCustomType<qint16, 64>		I16Size64;
typedef jCustomType<qint16, 128>	I16Size128;
typedef jCustomType<qint16, 256>	I16Size256;
typedef jCustomType<qint16, 512>	I16Size512;
typedef jCustomType<qint16, 1024>	I16Size1024;
typedef jCustomType<qint16, 2048>	I16Size2048;
typedef jCustomType<qint16, 4096>	I16Size4096;
typedef jCustomType<qint16, 8192>	I16Size8192;
typedef jCustomType<qint16, 16384>	I16Size16384;
typedef jCustomType<qint16, 32768>	I16Size32768;
typedef jCustomType<qint16, 65536>	I16Size65536;

typedef jCustomType<qint32, 1>		I32Size1;
typedef jCustomType<qint32, 2>		I32Size2;
typedef jCustomType<qint32, 4>		I32Size4;
typedef jCustomType<qint32, 8>		I32Size8;
typedef jCustomType<qint32, 16>		I32Size16;
typedef jCustomType<qint32, 32>		I32Size32;
typedef jCustomType<qint32, 64>		I32Size64;
typedef jCustomType<qint32, 128>	I32Size128;
typedef jCustomType<qint32, 256>	I32Size256;
typedef jCustomType<qint32, 512>	I32Size512;
typedef jCustomType<qint32, 1024>	I32Size1024;
typedef jCustomType<qint32, 2048>	I32Size2048;
typedef jCustomType<qint32, 4096>	I32Size4096;
typedef jCustomType<qint32, 8192>	I32Size8192;
typedef jCustomType<qint32, 16384>	I32Size16384;
typedef jCustomType<qint32, 32768>	I32Size32768;
typedef jCustomType<qint32, 65536>	I32Size65536;

typedef jCustomType<qint64, 1>		I64Size1;
typedef jCustomType<qint64, 2>		I64Size2;
typedef jCustomType<qint64, 4>		I64Size4;
typedef jCustomType<qint64, 8>		I64Size8;
typedef jCustomType<qint64, 16>		I64Size16;
typedef jCustomType<qint64, 32>		I64Size32;
typedef jCustomType<qint64, 64>		I64Size64;
typedef jCustomType<qint64, 128>	I64Size128;
typedef jCustomType<qint64, 256>	I64Size256;
typedef jCustomType<qint64, 512>	I64Size512;
typedef jCustomType<qint64, 1024>	I64Size1024;
typedef jCustomType<qint64, 2048>	I64Size2048;
typedef jCustomType<qint64, 4096>	I64Size4096;
typedef jCustomType<qint64, 8192>	I64Size8192;
typedef jCustomType<qint64, 16384>	I64Size16384;
typedef jCustomType<qint64, 32768>	I64Size32768;
typedef jCustomType<qint64, 65536>	I64Size65536;

typedef jCustomType<qreal, 1>		F32Size1;
typedef jCustomType<qreal, 2>		F32Size2;
typedef jCustomType<qreal, 4>		F32Size4;
typedef jCustomType<qreal, 8>		F32Size8;
typedef jCustomType<qreal, 16>		F32Size16;
typedef jCustomType<qreal, 32>		F32Size32;
typedef jCustomType<qreal, 64>		F32Size64;
typedef jCustomType<qreal, 128>		F32Size128;
typedef jCustomType<qreal, 256>		F32Size256;
typedef jCustomType<qreal, 512>		F32Size512;
typedef jCustomType<qreal, 1024>	F32Size1024;
typedef jCustomType<qreal, 2048>	F32Size2048;
typedef jCustomType<qreal, 4096>	F32Size4096;
typedef jCustomType<qreal, 8192>	F32Size8192;
typedef jCustomType<qreal, 16384>	F32Size16384;
typedef jCustomType<qreal, 32768>	F32Size32768;
typedef jCustomType<qreal, 65536>	F32Size65536;

#endif