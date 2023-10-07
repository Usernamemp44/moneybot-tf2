#pragma once
class ISurface
{
public:
	void UnlockCursor();
	void LockCursor();
	void SetCursorAlwaysVisible(bool b);
	static ISurface* factory();
};