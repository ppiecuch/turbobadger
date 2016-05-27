// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#include "tb_system.h"

#ifdef TB_SYSTEM_QT

#include <QClipboard>
#include <QApplication>
#include <stdio.h>

namespace tb {

// == TBClipboard =====================================

void TBClipboard::Empty()
{
}

bool TBClipboard::HasText()
{
	bool has_text = false;

	return has_text;
}

bool TBClipboard::SetText(const char *text)
{
	QApplication::clipboard()->setText(text);
	return true;
}

bool TBClipboard::GetText(TBStr &text)
{
	bool success = false;
	QString s = QApplication::clipboard()->text();
	return success;
}

} // namespace tb

#endif // TB_SYSTEM_QT
