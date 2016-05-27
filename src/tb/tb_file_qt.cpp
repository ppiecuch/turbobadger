// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#include "tb_system.h"

#ifdef TB_SYSTEM_QT

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <stdio.h>

namespace tb {

class TBQtFile : public TBFile
{
public:
	TBQtFile(QFile *f) : file(f) { Q_ASSERT(f); }
	virtual ~TBQtFile() { delete file; }

	virtual long Size()
	{
		return file->size();
	}
	virtual size_t Read(void *buf, size_t elemSize, size_t count)
	{
		return file->read((char*)buf, elemSize * count);
	}
private:
	QFile *file;
};

// static
TBFile *TBFile::Open(const char *filename, TBFileMode mode)
{
	switch (mode)
	{
	case MODE_READ:
	{
		QFile *f = new QFile(filename);
		if(f->exists() && f->open(QIODevice::ReadOnly))
			return new TBQtFile(f);
		delete f;
		f = new QFile(QString(":%1").arg(filename));
		if(f->exists() && f->open(QIODevice::ReadOnly))
			return new TBQtFile(f);
		qWarning() << "Failed to open file" << filename;
		delete f;
	}; break;
	default:
		break;
	}
	return nullptr;
}

} // namespace tb

#endif // TB_SYSTEM_QT
