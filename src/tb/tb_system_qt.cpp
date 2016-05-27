// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#include "tb_system.h"

#ifdef TB_SYSTEM_QT

#include <QDebug>
#include <QElapsedTimer>
#include <stdio.h>

#ifdef TB_RUNTIME_DEBUG_INFO

void TBDebugOut(const char *str)
{
	qDebug() << str;
}

#endif // TB_RUNTIME_DEBUG_INFO

namespace tb {

// == TBSystem ========================================

double TBSystem::GetTimeMS()
{
	static QElapsedTimer timer;
	if (!timer.isValid()) timer.start();
	return timer.elapsed();	
}

// == Implemented in port_qt
#if 0
void TBSystem::RescheduleTimer(double fire_time)
{
	
}
#endif

int TBSystem::GetLongClickDelayMS()
{
	return 500;
}

int TBSystem::GetPanThreshold()
{
	return 5 * GetDPI() / 96;
}

int TBSystem::GetPixelsPerLine()
{
	return 40 * GetDPI() / 96;
}

int TBSystem::GetDPI()
{
	// FIX: Implement!
	return 96;
}

} // namespace tb

#endif // QT_CORE_LIB
